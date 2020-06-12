using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace big_int_interface
{
    public class BigIntException : Exception
    {
        public BigIntException(string msg) : base(msg) {}
    }
    public class InvalidBigIntException : FormatException
    {
        public InvalidBigIntException() : base("Invalid BigInt") {}
    }

    public class TooBigException : Exception
    {
        public TooBigException(string msg) : base(msg) {}
    }
    public struct BigInt : IComparable<BigInt>, IEquatable<BigInt>
    {
        static class Sign {
            public const int Negative = -1;
            public const int Zero = 0;
            public const int Positive = 1;
        };
        List<byte> bytes;
        int sign;
        public List<byte> Bytes { get { return bytes; } }
        int Size {
            get { return bytes == null? 0 : bytes.Count; }
            set { Resize(value); }
        }

        public int ByteSize { get { return Size; } }

        public static BigInt Zero { get { return new BigInt(); } }
        public static BigInt One { get { return new BigInt(1); } }

        public void Resize(int sz)
        {
            if (bytes == null)
            {
                bytes = new List<byte>(Enumerable.Repeat((byte) 0, sz));
                return;
            }

            var prevSz = bytes.Count;
            if(sz < prevSz)
                bytes.RemoveRange(sz, prevSz - sz);
            else if(sz > prevSz)
            {
                if(sz > bytes.Capacity)
                    bytes.Capacity = sz;
                bytes.AddRange(Enumerable.Repeat((byte) 0, sz - prevSz));
            }
        }

        //removes the unused bytes
        void ClearZeroes()
        {
            int i = Size;
            for (; i > 0; --i)
            {
                if (bytes[i-1] != 0) break;
                // if (_begin[i-1]) break;
            }
            Size = i;
            if (i == 0) sign = Sign.Zero;
        }
        BigInt(int size, int sign)
        {
            bytes = new List<byte>(Enumerable.Repeat((byte) 0, size));
            this.sign = sign;
        }
        BigInt(List<byte> bytes, int sign)
        {
            this.bytes = bytes;
            this.sign = sign;
        }
        public BigInt(List<byte> bytes) : this(bytes, Sign.Positive) {}

        public BigInt(BigInt rhs) : this(new List<byte>(rhs.bytes), rhs.sign) {}
        // public BigInt() : this(0) {}
        private void SetFrom(BigInt rhs)
        {
            bytes = new List<byte>(rhs.bytes);
            sign = rhs.sign;
        }
        private void SetFromZero() {
            bytes = new List<byte>();
            sign = Sign.Zero;
        }
        private void SetFrom(long val)
        {
            bytes = new List<byte>();
            if (val == 0) {
                sign = Sign.Zero;
                // clearMemory();
                return;
            }
            if (val < 0) {
                sign = Sign.Negative;
                val = -val;
            }
            else {
                sign = Sign.Positive;
            }

            for (;;) {
                bytes.Add((byte)val);
                val >>= 8;
                if (val == 0) break;
            }
        }

        public BigInt(long val)
        {
            bytes = null;
            sign = Sign.Zero;
            SetFrom(val);
        }
        public BigInt(string str)
        {
            this.sign = Sign.Zero;
            this.bytes = new List<byte>();

            var sign = Sign.Positive;
            if (string.IsNullOrEmpty(str))
                throw new InvalidBigIntException();

            int i = 0;
            switch (str[0])
            {
                case '-':
                    sign = Sign.Negative;
                    goto case '+';
                case '+':
                    ++i;
                    if (i >= str.Length)
                        throw new InvalidBigIntException();
                    break;
            }

            for (;;) {
                char c = str[i];
                uint digit = c - ((uint)'0');
                if (digit >= 10)
                    throw new InvalidBigIntException();
                SmallMulImpl(this, 10, ref this);
                this += (BigInt) digit;

                ++i;
                if (i >= str.Length)
                    break;
            }
            //check for 0?
            this.sign = sign;
        }

        public bool IsZero { get { return sign == Sign.Zero; } }

        public static explicit operator BigInt(bool val)
        {
            return new BigInt(val? One: Zero);
        }
        public static explicit operator BigInt(string str)
        {
            return new BigInt(str);
        }
        public static implicit operator BigInt(long val)
        {
            return new BigInt(val);
        }

        public static explicit operator long(BigInt a)
        {
            return a.ToLong();
        }
        public static explicit operator int(BigInt a)
        {
            return a.ToInt();
        }
        public int ToInt(string msg = "Number too big for int")
        {
            if (Size > sizeof(int))
                throw new TooBigException(msg);
            return (int) ToLong(msg);
        }
        public long ToLong(string msg = "Number too big for int64")
        {
            if (sign == Sign.Zero) return 0;
            if (Size > sizeof(long))
                throw new TooBigException(msg);
            long res = 0;
            int i = 0;
            foreach (var b in bytes)
            {
                res |= (long)(b << i);
                i += 8;
            }
            return (sign == Sign.Negative) ? -res : res;
        }
        public string ToHex()
        {
            char hexChar(int i)
            {
                if (i < 10) return (char) ((int) '0' + i);
                return (char)((int)'A' - 10 + i);
            }
            string res;
            switch (sign)
            {
                case Sign.Zero: return "0";
                case Sign.Negative: res = "-"; break;
                default:
                case Sign.Positive: res = "+"; break;
            }
            for (int i = Size-1; i >= 0; --i)
            {
                res += hexChar(bytes[i] >> 4);
                res += hexChar(bytes[i] & 0xF);
            }
            return res;
        }
        public string ToDec()
        {
            string res;
            switch (sign) {
                case Sign.Zero: return "0";
                case Sign.Negative: res = "-"; break;
                default: res = ""; break;
            }
            BigInt a = new BigInt(this);
            //we can have at most 3 digits per byte
            char[] c = new char[a.Size*3];
            int i = 0;

            BigInt r = new BigInt();
            BigInt ten = (BigInt)10;
            while (!a.IsZero) {
                DivRem(a, ten, ref a, out r);
                c[i++] =  (r.bytes.Count == 0) ? '0' : (char) ((int)'0' + r.bytes[0]);
            }
            do
            {
                res += c[--i];
            }
            while (i != 0);

            return res;
        }

        public override string ToString()
        {
            return ToDec();
        }
        public static BigInt operator+(BigInt a) { return a; }
        public static BigInt operator-(BigInt a) {
            BigInt res = new BigInt(a);
            res.sign = -a.sign;
            return res;
        }

        void OrEquals(BigInt rhs) {
            if (sign == Sign.Negative || rhs.sign == Sign.Negative)
                throw new BigIntException("Can't bitwise OR negative numbers");
            if (rhs.IsZero) return;
            if (IsZero) {
                SetFrom(rhs);
                return;
            }

            if (Size < rhs.Size) {
                Resize(rhs.Size);
            }
            for (int i = 0; i < rhs.Size; ++i) {
                bytes[i] |= rhs.bytes[i];
            }
            ClearZeroes();
        }

        public static BigInt operator|(BigInt lhs, BigInt rhs)
        {
            if (lhs.Size < rhs.Size)
            {
                BigInt res = new BigInt(rhs);
                res.OrEquals(lhs);
                return res;
            }
            else
            {
                BigInt res = new BigInt(lhs);
                res.OrEquals(rhs);
                return res;
            }
        }

        void XorEquals(BigInt rhs) {
            if (sign == Sign.Negative || rhs.sign == Sign.Negative)
                throw new BigIntException("Can't bitwise XOR negative numbers");
            if (rhs.IsZero) return;
            if (IsZero) {
                SetFrom(rhs);
                return;
            }

            if (Size < rhs.Size) {
                Resize(rhs.Size);
            }
            for (int i = 0; i < rhs.Size; ++i) {
                bytes[i] ^= rhs.bytes[i];
            }
            ClearZeroes();
        }

        public static BigInt operator^(BigInt lhs, BigInt rhs)
        {
            if (lhs.Size < rhs.Size)
            {
                BigInt res = new BigInt(rhs);
                res.XorEquals(lhs);
                return res;
            }
            else
            {
                BigInt res = new BigInt(lhs);
                res.XorEquals(rhs);
                return res;
            }
        }

        public static BigInt operator&(BigInt lhs, BigInt rhs)
        {
            if (lhs.IsZero || rhs.IsZero) return new BigInt();
            int min = Math.Min(rhs.Size, lhs.Size);
            var res = new BigInt(min, Sign.Positive);

            for (int i = 0; i < min; ++i) {
                byte val = (byte)(lhs.bytes[i] & rhs.bytes[i]);
                res.bytes[i] = val;
            }
            res.ClearZeroes();
            return res;
        }
        void LshEquals(int rhs)
        {
            LshEqualsImpl(rhs, Size);
            ClearZeroes();
        }
        void LshEqualsImpl(int rhs, int size)
        {
            int bigShift   = rhs / 8;
            int smallShift = rhs % 8;

            if (rhs < 0) throw new BigIntException("Negative shift");
            Resize(size + bigShift + (smallShift != 0? 1: 0));

            int carry = 0;
            if (smallShift != 0) {
                for (int j = 0; j < Size; ++j) {
                    carry = (bytes[j] << smallShift) | carry;
                    bytes[j] = (byte) carry;
                    carry >>= 8;
                }
            }
            int i = this.Size - 1;
            for (; i >= bigShift; --i) {
                bytes[i] = bytes[i-bigShift];
            }
            for (; i >= 0; --i)
                bytes[i] = 0;
        }
        public static BigInt Lsh(BigInt lhs, BigInt rhs)
        {
            return lhs << (int)rhs.ToInt("Can't left shift by that much");
        }

        public static BigInt operator<<(BigInt lhs, int rhs)
        {
            BigInt res = new BigInt(lhs);
            res.LshEquals(rhs);
            return res;
        }

        public void RshEquals(int rhs)
        {
            if (rhs < 0) throw new BigIntException("Negative shift");

            var bigShift = rhs / 8;
            var smallShift = rhs % 8;
            if (bigShift >= Size)
            {
                SetFromZero();
                return;
            }
            int newSize = Size - bigShift;

            int carry = 0;
            bytes[0] = (byte) (bytes[bigShift] >> smallShift);
            for (int i = 1; i < newSize; ++i) {
                carry = (bytes[i+bigShift] << (8 - smallShift));
                bytes[i] = (byte) (carry >> 8);
                bytes[i-1] |= (byte) carry;
            }
            Resize(newSize);

            ClearZeroes();
        }
        public void RshEquals(BigInt rhs)
        {
            if (rhs.Size > sizeof(int))
            {
                SetFromZero();
                return;
            }
            var rhs_ = rhs.ToInt();
            if (rhs_ < 0) throw new BigIntException("Negative shift");
            RshEquals(rhs_);
        }

        public static BigInt Rsh(BigInt lhs, BigInt rhs)
        {
            if (rhs.Size >= sizeof(int)) return Zero;
            return lhs >> (int)rhs;
        }
        public static BigInt operator>>(BigInt lhs, int rhs)
        {
            var res = new BigInt(lhs);
            res.RshEquals(rhs);
            return res;
        }
        public static bool operator==(BigInt rhs, BigInt lhs)
        {
            if (lhs.sign != rhs.sign) return false;
            if (lhs.sign == Sign.Zero) return true;
            if (lhs.Size != rhs.Size) return false;

            for (int i = 0; i < lhs.Size; ++i)
            {
                if (lhs.bytes[i] != rhs.bytes[i]) return false;
            }
            return true;
        }
        public static bool operator!=(BigInt rhs, BigInt lhs)
        {
            return !(rhs==lhs);
        }

        public override bool Equals(object obj)
        {
            if (obj is BigInt)
                return Equals((BigInt)obj);
            return false;
        }
        public bool Equals(BigInt bi)
        {
            return bi == this;
        }
        
        public override int GetHashCode()
        {
            if (bytes == null) return 0;
            return sign * bytes.GetHashCode();
        }
        public static bool operator<=(BigInt a, BigInt b) { return a.CompareTo(b) <= 0; }
        public static bool operator< (BigInt a, BigInt b) { return a.CompareTo(b) < 0; }
        public static bool operator> (BigInt a, BigInt b) { return a.CompareTo(b) > 0; }
        public static bool operator>=(BigInt a, BigInt b) { return a.CompareTo(b) >= 0; }

        public int CompareTo(BigInt rhs)
        {
            // might return -2 or 2, but we care about the sign anyway
            if (sign != rhs.sign) return sign - rhs.sign;

            if (IsZero) return 0;
            return AbsCmp(this, rhs) * sign;
        }

        public static int AbsCmp(BigInt lhs, BigInt rhs)
        {
            if (lhs.Size > rhs.Size) return 1;
            if (lhs.Size < rhs.Size) return -1;

            for (int i = lhs.Size-1; i >= 0; --i)
            {
                if (lhs.bytes[i] < rhs.bytes[i]) return -1;
                if (lhs.bytes[i] > rhs.bytes[i]) return 1;
            }
            return 0;
        }

        enum Op {
            Add, Sub
        }
        static BigInt Add(Op op, BigInt a, BigInt b)
        {
            bool bSign = !((op == Op.Add) ^ (b.sign >= 0));
            bool aSign = a.sign >= 0;


            if (a.sign == Sign.Zero) {
                BigInt res = new BigInt(b);
                if (op != Op.Add) res.sign = -b.sign;
                return res;
            }
            if (b.sign == Sign.Zero) {
                BigInt res = new BigInt(a);
                //TODO CHECK FOR res = a; (we should call 'op=')
                return res;
            }

            if (aSign == bSign) {
                BigInt res = new BigInt();
                res.sign = a.sign;
                if (a.Size > b.Size) {
                    AddImpl(Op.Add, a, b, ref res);
                    return res;
                }
                AddImpl(Op.Add, b, a, ref res);
                return res;
            }
            //we must be subtracting if we got here
            int cmp = AbsCmp(a, b);
            if (cmp == 0)
                return Zero;
            bool delta = cmp >= 0; // >= from the table
            
            bool inverted = aSign ^ bSign ^ delta; // do b - a
            bool minus = bSign == delta; // do -(whatever)
            {
                BigInt res = new BigInt();
                res.sign = minus ? Sign.Negative : Sign.Positive;
                if (inverted) {
                    AddImpl(Op.Sub, b, a, ref res);
                    return res;
                }
                AddImpl(Op.Sub, a, b, ref res);
                return res;
            }
            /*

            b = b.sign * (isNegative? -1: 1)
            a = a.sign
            | a | b | a>0 | b>0 | >= | minus | inverted | op     | preview |
            |---+---+-----+-----+----+-------+----------+--------+---------|
            | + | + |   1 |   1 |  ? |     0 |        0 | a +b   | AA+BB   |
            | + | - |   1 |   0 |  1 |     0 |        0 | a-b    | AA-B    |
            | + | - |   1 |   0 |  0 |     1 |        1 | -(b-a) | A-BB    |
            | - | + |   0 |   1 |  1 |     1 |        0 | -(a-b) | -AA+B   |
            | - | + |   0 |   1 |  0 |     0 |        1 | b-a    | -A+BB   |
            | - | - |   0 |   0 |  ? |     1 |        0 | -(a+b) | -AA-BB  |

            inverted = a.s^b.s^(>=) # where ? = 0m
            minus = (a>0) == (>=)
            if (b.sign == b.sign) return (AA+BB) *sign
            */
        }

        static void AddImpl(Op op, BigInt a, BigInt b, ref BigInt res) {
            AddImpl(op, a, b, ref res, a.Size, b.Size);
        }
        static BigInt AddImpl(Op op, BigInt a, BigInt b, ref BigInt res,
                              int aSize, int bSize) {

            // redundant zeroes will be removed
            if (op == Op.Add)
                res.Resize(aSize + 1);
            else res.Resize(aSize);

            int carry = 0;
            int i = 0;
            if (op == Op.Add) {
                for (; i < bSize; ++i) {
                    carry += a.bytes[i] + b.bytes[i];
                    res.bytes[i] = (byte)carry;
                    carry >>= 8;
                }
                if (aSize > i) {
                    if (carry != 0) {
                        for (;;) {
                            if (i >= aSize)//+1)
                            {
                                res.bytes[i] = (byte) carry;
                                goto end;
                            }

                            res.bytes[i] = (byte) (a.bytes[i] + carry);
                            var rbyte = res.bytes[i];
                            ++i;
                            if (rbyte != 0)
                                break;
                        }
                    }
                }
                else {
                    res.bytes[i] = (byte) carry;
                    ++i;
                }

                for (; i < aSize; ++i) {
                    res.bytes[i] = a.bytes[i];
                }
            }
            else {
                for (; i < bSize; ++i) {
                    int r = a.bytes[i] - b.bytes[i] - carry;
                    if (r < 0) {
                        carry = 1;
                        r = 0x0100 + r;
                    }
                    else carry = 0;
                    res.bytes[i] = (byte) r;
                }

                if (carry != 0) {
                    for (;;) {
                        if (i >= aSize) goto end;
                        var abyte = a.bytes[i];
                        res.bytes[i] = (byte) (a.bytes[i] - carry);
                        ++i;
                        if (abyte != 0) break;
                    }
                }
                for (; i < aSize; ++i) {
                    res.bytes[i] = a.bytes[i];
                }
            }

        end:
            res.ClearZeroes();
            return res;
        }

        public static BigInt operator+(BigInt lhs, BigInt rhs)
        {
            return Add(Op.Add, lhs, rhs);
        }
        public static BigInt operator-(BigInt lhs, BigInt rhs)
        {
            return Add(Op.Sub, lhs, rhs);
        }

        public static BigInt operator/(BigInt a, BigInt b) {
            BigInt q = new BigInt(), r;
            DivRem(a, b, ref q, out r);
            return q;
        }

        public static BigInt operator%(BigInt a, BigInt b) {
            BigInt q = new BigInt(), r;
            DivRem(a, b, ref q, out r);
            return r;
        }
        public static void DivRem(BigInt a, BigInt b, ref BigInt q, out BigInt r)
        {
            if (b.IsZero) throw new DivideByZeroException();

            if (q.bytes == b.bytes)
                throw new BigIntException("divRem: b, q and r must be "+
                                          "references to different objects");
            if (AbsCmp(b, a) > 0) {
                //we don't do q = 0 first because it might be the same data as a
                r = new BigInt(a);
                q = Zero;
                return;
            }
            var bSize = b.Size;
            var aSize = a.Size;

            // we have r < b, so we alocate memory for the worst case
            var rSize = bSize+1;
            var qSize = aSize;
            
            var rSign = a.sign;
            var qSign = a.sign * b.sign;

            r = new BigInt(rSize, Sign.Positive);
            // r.Resize(rSize);
            // for (int i = 0; i < rSize; ++i) r.bytes[i] = 0;

            q.Resize(qSize);

            int AbsCmpRemainder(BigInt r_, BigInt b_)
            {
                if (r_.bytes[bSize] != 0)
                    return 1;
                for (int i = bSize-1; i >= 0; --i)
                {
                    if (r_.bytes[i] < b_.bytes[i])
                        return -1;
                    if (r_.bytes[i] > b_.bytes[i])
                        return 1;
                }
                return 0;
            }

            r.sign = rSign;
            q.sign = qSign;

            for (int i = aSize -1; i >= 0; --i)
            {
                byte abyte = a.bytes[i];
                q.bytes[i] = 0;
                for (int bitIndex = 7; bitIndex >= 0; --bitIndex)
                {
                    r.LshEqualsImpl(1, bSize);

                    r.bytes[0] |= (byte) ((abyte >> bitIndex) & 1);
                    if (AbsCmpRemainder(r, b) >= 0) {
                        AddImpl(Op.Sub, r, b, ref r, rSize, bSize);
                        // set bit
                        q.bytes[i] |= (byte) (1 << bitIndex);
                    }
                }
            }
            r.sign = rSign;
            q.sign = qSign;
            r.ClearZeroes();
            q.ClearZeroes();
        }
        
        public static BigInt operator*(BigInt a, BigInt b)
        {
            BigInt res = new BigInt();
            Mul(a, b, ref res);
            return res;
        }

        public static void Mul(BigInt a, BigInt b, ref BigInt res)
        {
            var sign = a.sign * b.sign;
            MulImpl(a, b, ref res);
            res.sign = sign;
        }

        //TODO
        public void SplitNum(out BigInt low, out BigInt high, int n) {
            low = new BigInt(n, Sign.Positive);
            high = new BigInt(Size- n, Sign.Positive);
            // low.Resize(n);
            // high.Resize(Size - n);
            // low.sign = high.sign = Sign.Positive;
            int i = 0;

            for (; i < n; ++i)
                low.bytes[i] = bytes[i];

            for (; i < Size; ++i)
                high.bytes[i-n] = bytes[i];
        }

        // doesn't care about signs, assumes positive
        //TODO
        public static void SmallMulImpl(BigInt a, byte b, ref BigInt res)
        {
            var aSize = a.Size;
            // ceil(log_256(a*b)) = ceil(log_256(a)) + ceil(log_256(b)) <=
            //      <= ceil(log_256(a)) + ceil(log_256(255)) = a.size + 1
            res.Resize(a.Size + 1);
            int carry = 0;

            for (int i = 0; i < aSize; ++i)
            {
                carry += a.bytes[i] * b;
                res.bytes[i] = (byte) carry;
                carry >>= 8;
            }
            res.bytes[aSize] = (byte) carry;
            res.sign = Sign.Positive;

            res.ClearZeroes();

            return;
        }

        // doesn't care about signs
        //https://en.wikipedia.org/wiki/Karatsuba_algorithm
        static void MulImpl(BigInt a, BigInt b, ref BigInt res)
        {
            if (a.Size <= 1)
            {
                if (a.IsZero) res.SetFromZero();
                else SmallMulImpl(b, a.bytes[0], ref res);
                return;
            }
            if (b.Size <= 1)
            {
                if (b.IsZero) res.SetFromZero();
                else SmallMulImpl(a, b.bytes[0], ref res);
                return;
            }
            var m = Math.Min(a.Size, b.Size);
            var m2 = m / 2;

            BigInt lowA, highA, lowB, highB;

            a.SplitNum(out lowA, out highA, m2);
            b.SplitNum(out lowB, out highB, m2);

            // BigInt& z2 = res;
            BigInt z0 = new BigInt(), z1 = new BigInt();

            MulImpl(highA, highB, ref res);
            MulImpl(lowA, lowB, ref z0);

            lowA += highA;
            lowB += highB;

            MulImpl(lowA, lowB, ref z1);

            z1 -= res;
            z1 -= z0;

            res.LshEquals(m2*2*8);
            z1.LshEquals(m2*8);

            //res <<= m2*2*8;
            //z1 <<= m2*8;

            res += z1;
            res += z0;
        }
        public static BigInt Pow(BigInt a, BigInt b)
        {
            if (b.sign == Sign.Negative)
                throw new BigIntException("Can't raise integers to a negative power");
            BigInt res = One;
            BigInt x = new BigInt(a);
            BigInt n = new BigInt(b);
            while (!n.IsZero) {
                if (n.bytes[0] % 2 == 1) // n is odd
                    res *= x;
                x *= x;
                n.RshEquals(1); // n /= 2;
            }
            return res;
        }
    }
}

