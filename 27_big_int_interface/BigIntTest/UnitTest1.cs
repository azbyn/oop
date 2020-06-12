using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using System;
using System.Linq;

using BigIntInterface;

namespace BigIntTest
{
    [TestClass]
    public class BigIntTester
    {
        // [TestMethod] public void TestGeneral()
        // {
        //     BigInt a = (BigInt)213;
        //     BigInt b = new BigInt();
        //     BigInt c = new BigInt(213);
        //     BigInt d = new BigInt("213");
        //     BigInt e = (BigInt) "213";
        // }
        [TestMethod] public void TestResizeMove()
        {
            BigInt b = (BigInt)0x2213;
            Assert.AreEqual(b.ByteSize, 2);

            b.Resize(16);
            Assert.AreEqual(b.ByteSize, 16);
            b.Resize(7);
            Assert.AreEqual(b.ByteSize, 7);
            BigInt a = new BigInt();
            Assert.AreEqual(a.ByteSize, 0); //error?
            a = b;
            b.Bytes[0] = 42;
            Assert.AreEqual(a.Bytes[0], (byte) 42);
        }
        static void AssertListsAreEqual(string msg, List<byte> a, List<byte> b)
        {
            bool IsNullOrEmpty(List<byte> s)
            {
                return s == null || (s.Count == 0);
            }
            if (IsNullOrEmpty(a) || IsNullOrEmpty(b))
            {
                Assert.AreEqual(IsNullOrEmpty(a), IsNullOrEmpty(b), msg);
                return;
            }
            string toStr(List<byte> l)
            {
                string res = "{";
                for (int i = 0; i < l.Count; ++i)
                    res += l[i]+",";
                return res + "}";
            }
            if (a.Count != b.Count)
            {
                Assert.AreEqual(a.Count, b.Count,
                                $"{msg};{a.Count}!={b.Count} a:{toStr(a)}, b:{toStr(b)}");
                return;
            }
            int ith = -1;
            for (int i = 0; i < a.Count; ++i)
            {
                if (a[i] != b[i]) ith = i;
            }
            Assert.AreEqual(-1, ith, $"{msg};{ith}-th differs: '{toStr(a)}' != '{toStr(b)}'");
        }
        static void AssertBytes(BigInt a, List<byte> b)
        {
            AssertListsAreEqual("", a.Bytes, b);
        }
        static void AssertEquals(BigInt a, long i)
        {
            BigInt b = (BigInt) i;
            AssertListsAreEqual("",a.Bytes, b.Bytes);
        }

        static void AssertBytes(string msg, BigInt a, List<byte> b)
        {
            AssertListsAreEqual(msg, a.Bytes, b);
        }
        static void AssertEquals(string msg, BigInt a, long i)
        {
            BigInt b = (BigInt) i;
            AssertListsAreEqual(msg, a.Bytes, b.Bytes);
        }
        [TestMethod] public void TestOr()
        {
            BigInt a = (BigInt) 0x0122;
            BigInt b = new BigInt(new List<byte>{0,0,0,0xFF,1,0,2,1,2});
            BigInt c = (BigInt) 1;
            a |= b;
            AssertBytes(a, new List<byte>{0x22,1,0,0xFF,1,0,2,1,2});
            b |= c;
            AssertBytes(b, new List<byte>{1,0,0,0xFF,1,0,2,1,2});
        }

        [TestMethod] public void TestXor()
        {
            AssertEquals((BigInt) 42 ^ (BigInt) 0xFA, 42^0xFA);
        }

        [TestMethod] public void TestPow()
        {
            AssertEquals(BigInt.Pow(10, 3), 1000);
            AssertEquals(BigInt.Pow(-1, 10), 1);
            AssertEquals(BigInt.Pow(-2, 10), 1024);
        }
        [TestMethod] public void TestAnd()
        {
            BigInt a = (BigInt) 0x0122;
            AssertBytes(a & (BigInt) 0x0300, new List<byte>{0, 1});
            AssertBytes(a | (BigInt) 0x0300, new List<byte>{0x22, 3});
            Assert.IsTrue((a & (BigInt) 0x0200).IsZero);
            AssertBytes(a & (BigInt) 0x0F, new List<byte>{2});


            var b = new BigInt(new List<byte>{0xFF,0,   0,0xFF, 1,0,   2,1, 2});
            var c = new BigInt(new List<byte>{0xFF,0,0xFF,0xFF, 1,0,0xFF,1, 0});

            AssertBytes(a & b, new List<byte>{0x22});
            AssertBytes(c & b, new List<byte>{0xFF,0,0,0xFF, 1,0,2,1});
        }
        [TestMethod] public void TestLsh()
        {
            BigInt a = (BigInt) 0x0100;
            AssertEquals(a << 2, 0x0100 << 2);
            AssertEquals(a << 9, 0x0100 << 9);
        }
        [TestMethod] public void TestRsh()
        {
            BigInt a = (BigInt) 0x0A0F;
            AssertEquals(((BigInt) 0x102)>> 8, 1);

            AssertEquals(a >> 2, 0x0A0F >> 2);
            AssertEquals(a >> 9, 0x0A0F >> 9);
        }
        static void AssertEquals<T>(string msg, T val, T expected) {
            Assert.AreEqual(expected, val, msg);
        }
        [TestMethod] public void TestCmp()
        {
            int[] arr = new int[] {
                0,
                2,-2,
                0x2211, -0x2211,
                0x323232,
                -0x323232,
            };
            foreach (int ia in arr) {
                BigInt a_ = (BigInt) ia;
                foreach (int ib in arr) {
                    BigInt b_ = (BigInt) ib;
                    AssertEquals("cmp", a_ == b_, ia == ib);
                    AssertEquals("cmp", a_ != b_, ia != ib);
                    AssertEquals("cmp", a_ <= b_, ia <= ib);
                    AssertEquals("cmp", a_ >  b_, ia >  ib);
                }
            }
            BigInt a = (BigInt)0x0A_0F22;
            BigInt b = (BigInt)0x0A_0F22;
            BigInt c = (BigInt)0x0;
            BigInt d = (BigInt)(-0x0A_0F22);
            var two = (BigInt)2;

            AssertEquals("cmp", a == b, true);
            AssertEquals("cmp", a == (BigInt) 0x0A_0F22, true);
            AssertEquals("cmp", a != b, false);
            AssertEquals("cmp", a <= b, true);
            AssertEquals("cmp", a <= two, false);
            AssertEquals("cmp", a > two, true);
            AssertEquals("cmp", a == two, false);
            AssertEquals("cmp", a < (BigInt)0xA0_ABCD, true);
            AssertEquals("cmp", a < (BigInt)0xABCD, false);
            AssertEquals("cmp", a < (BigInt)0x100ABCD, true);
            AssertEquals("cmp", c.CompareTo(d), 1);

            AssertEquals("cmp", a < (BigInt)0x09_ABCD, false);
        }
        [TestMethod] public void TestToInt()
        {
            BigInt a = (BigInt) 0x0A_0F22;
            AssertEquals("toInt", a.ToInt(), 0x0A_0F22);
            AssertEquals("toInt", new BigInt(-0x0A_0F22).ToInt(), -0x0A_0F22);
            AssertEquals("toInt", new BigInt(42).ToInt(), 42);
            AssertEquals("toInt", new BigInt().ToInt(), 0);
            BigInt b = new BigInt(new List<byte> {0xFF,0,0,0xFF, 1,0,2,1, 2});
            Assert.ThrowsException<TooBigException>(() => { b.ToInt(); });
        }
        [TestMethod] public void TestAbsCmp()
        {
            AssertEquals("cmp", BigInt.AbsCmp((BigInt)0xffff, (BigInt)2), 1);
            AssertEquals("cmp", BigInt.AbsCmp((BigInt)2, (BigInt)0xffff), -1);
            AssertEquals("cmp", BigInt.AbsCmp((BigInt)0xffff,(BigInt) 0xffff), 0);

            AssertEquals("cmp", BigInt.AbsCmp((BigInt)0xffff, (BigInt) (-02)), 1);
            AssertEquals("cmp", BigInt.AbsCmp((BigInt)02, (BigInt) (-0xffff)), -1);
            AssertEquals("cmp", BigInt.AbsCmp((BigInt)0xffff, (BigInt) (-0xffff)), 0);
        }

        static void TestAddImpl(long ai, long bi)
        {
            AssertEquals($"{ai}-{bi}", ((BigInt)ai - (BigInt)bi).ToInt(), ai - bi);
            AssertEquals($"{ai}+{bi}", ((BigInt)ai + (BigInt)bi).ToInt(), ai + bi);
        }
        static void TestAddImpl(long ai, long bi, BigInt a, BigInt b)
        {
            AssertEquals($"{ai}-{bi}", (int)(a-b), ai - bi);
            AssertEquals($"{ai}+{bi}", (int)(a+b), ai + bi);
        }


        [TestMethod] public void TestAdd()
        {
            TestAddImpl(65580, 98);
            TestAddImpl(0xd6, 0x14e05);
            TestAddImpl(0x10_00_00_00, 1);
            TestAddImpl(0x15a03, 0xcf);
            TestAddImpl(199, -65536);
        }
        Random rnd = new Random();
        int RandomInt()
        {
            return rnd.Next(int.MinValue/2, int.MaxValue/2);
        }
        [TestMethod] public void TestAddRandoms1()
        {
            for (int i = 0; i < 1000000; ++i) {
                TestAddImpl(RandomInt(), RandomInt());
            }
        }

        [TestMethod] public void TestAddArray()
        {
            int[] arr = new int[] {
                0,
                2,-2,
                0x2211, -0x2211,
                0x323232,
                -0x323232,
            };
            foreach (int ai in arr)
            {
                var a = (BigInt) ai;
                var minusA = -a;
                AssertEquals("neg", (int)(minusA), -ai);
                foreach (int bi in arr)
                {
                    var b = (BigInt) bi;
                    TestAddImpl(ai, bi, a, b);
                    TestAddImpl(-ai, bi, minusA, b);
                }
            }
        }
        [TestMethod] public void TestAddMisc()
        {
            BigInt a = (BigInt) 231;
            BigInt b = (BigInt) 231;
            AssertEquals("add", (int)(new BigInt(0x21) + new BigInt(0x1200)), 0x1221);
            AssertEquals("add", (int)((BigInt)(0x2101) +(BigInt)(0x11)), 0x2101 + 0x11);
            AssertEquals("add", (int)(a+b), 231+231);
            AssertEquals("add", (int)((BigInt)(13)+(BigInt)(-12)), 1);
            AssertEquals("add", (int)((BigInt)(13)-(BigInt)(12)), 1);
            AssertEquals("add", (int)((BigInt)(12)-(BigInt)(13)), -1);
            AssertEquals("add", (int)((BigInt)(13)-(BigInt)(12)), 1);
            AssertEquals("add", (int)((BigInt)(12)-(BigInt)(12)), 0);
            AssertEquals("add", (int)((BigInt)(12)-(BigInt)(-12)), 24);
            AssertEquals("add", (int)((BigInt)(-12)-(BigInt)(-12)), 0);
            AssertEquals("add", (int)((BigInt)(-12)+(BigInt)(-12)), -24);

            AssertEquals("add", (int)((BigInt)0x21 +(BigInt)(-0x1212)), 0x21 - 0x1212);
            AssertEquals("add", (int)((BigInt)(-0x1212)+ (BigInt)0x21), 0x21 - 0x1212);
            AssertEquals("add", (int)((BigInt)0x1212-(BigInt)0x12), 0x1200);
            a -= a;
            AssertEquals("add", (int)a, 0);
        }


        void TestDivImpl1(long ai, long bi) {
            BigInt a = (BigInt)ai;
            BigInt b = (BigInt)bi;
            BigInt q = new BigInt();
            BigInt r;
            if (bi == 0)
            {
                Assert.ThrowsException<DivideByZeroException>(
                        () => { BigInt.DivRem(a, b, ref q, out r); });
                Assert.ThrowsException<DivideByZeroException>(
                        () => { BigInt.DivRem(a, b, ref a, out r); });
                return;
            }
            BigInt.DivRem(a, b, ref q, out r);
            AssertEquals($"{ai}/{bi} = {ai/bi}", q, ai/bi);
            AssertEquals($"{ai}%{bi} = {ai%bi}", r, ai%bi);
        }

        void TestDivImpl2(long ai, long bi) {
            BigInt a = (BigInt) ai;
            BigInt b = (BigInt) bi;
            if (bi == 0)
            {
                Assert.ThrowsException<DivideByZeroException>(() => a/b);
                Assert.ThrowsException<DivideByZeroException>(() => a%b);
                return;
            }
            AssertEquals($"{ai}/{bi} = {ai/bi}", a/b, ai/bi);
            AssertEquals($"{ai}%{bi} = {ai%bi}", a%b, ai%bi);
        }
        [TestMethod] public void TestDivArr()
        {
            TestDivImpl1(1, 1);
            int[] nums = new int[] {
                0, 1, -1,
                2, -2,
                3, -3,
                21, -21,
                42, -42,
                0x111, -0x111
            };
            foreach (int ai in nums)
            {
                foreach (int bi in nums)
                {
                    TestDivImpl1(ai, bi);
                    TestDivImpl2(ai, bi);
                }
            }
        }
        [TestMethod] public void TestDivRand()
        {
            for (int i = 0; i < 10000; ++i)
            {
                var ai = RandomInt();
                var bi = RandomInt() / 0xFFFF;
                TestDivImpl1(ai, bi);
                TestDivImpl2(ai, bi);
            }
            for (int i = 0; i < 10000; ++i)
            {
                var bi = RandomInt();
                var ai = RandomInt();
                TestDivImpl1(ai, bi);
                TestDivImpl2(ai, bi);
            }
        }
        [TestMethod] public void TestSmallMul()
        {
            void T(long ai, byte bi) 
            {
                BigInt a = (BigInt) ai;
                BigInt r = new BigInt();
                BigInt.SmallMulImpl(a, bi, ref r);

                AssertEquals($"{ai}*{bi}", r, (long) ai*bi);
                BigInt.SmallMulImpl(a, bi, ref a);
                AssertEquals($"{ai}*{bi}", a, (long) ai*bi);
            }
            T(0x10_00_00, 0xd0);
            T(0x10_00_00_00, 0xd0);

            for (int i = 0; i < 1000; ++i) {
                uint a = (uint) RandomInt();
                byte b = (byte) RandomInt();
                T(a, b);
            }
        }
        [TestMethod] public void TestSplitNum()
        {
            BigInt h, l;
            new BigInt(0xab_cd_ef).SplitNum(out h, out l, 2);
            AssertEquals("h", h, 0xcd_ef);
            AssertEquals("l", l, 0xab);
        }
        void TestMulImpl(long ai, long bi)
        {
            BigInt a = ai;
            BigInt b = bi;
            AssertEquals($"{ai}*{bi}", a*b, ai*bi);
        }
        [TestMethod] public void TestMul()
        {
            int[] nums = new int[] {
                0, 1, -1,
                2, -2,
                3, -3,
                21, -21,
                42, -42,
                0x111, -0x111,
                0xFFF213, -0xFFF213
            };
            TestMulImpl(0x111, 0x111);
            
            foreach (int ai in nums) {
                foreach (int bi in nums) {
                    TestMulImpl(ai, bi);
                }
            }
        }
        [TestMethod] public void TestMulRand()
        {
            for (int i = 0; i < 100000; ++i) {
                short ai = (short) RandomInt();
                long bi = RandomInt() / 0xFFFF;
                TestMulImpl(ai, bi);
            }
            for (int i = 0; i < 100000; ++i) {
                int bi = RandomInt();
                int ai = RandomInt();
                TestMulImpl(ai, bi);
            }
        }
        [TestMethod] public void TestFromStr()
        {
            void T(string str, long ai) {
                BigInt a = (BigInt)(str);
                AssertEquals($"'{str}'", a, ai);
            };
            string[] wrong = new string[] {
                "+", "-", "a", "sa21", "", "    ", " ", " a", "ö"
            };

            foreach (var s in wrong) {
                Assert.ThrowsException<InvalidBigIntException>(() => new BigInt(s));
            }

            T("1", 1);
            T("+1", 1);
            T("-1", -1);
            T("+0", 0);
            T("-0", 0);
            T("0", 0);
            T("-123456789", -123456789);
            T("123456789", 123456789);
            T("+123456789", 123456789);

            T("123", 123);
            T("-123", -123);
        }
        [TestMethod] public void TestToStr()
        {
            void T(string res, long ai)
            {
                var str = new BigInt(ai).ToString();
                Assert.AreEqual(ai.ToString(), str);
            }
            T("1", 1);
            T("-1", -1);
            T("0", 0);
            T("-123456789", -123456789);
            T("123456789", 123456789);

            T("123", 123);
            T("-123", -123);
        }
        [TestMethod] public void TestIo()
        {
            void T(string fromStr, string cmp)
            {
                BigInt a = (BigInt) fromStr;

                var res = a.ToString();
                Assert.AreEqual(cmp, res);
            }

            char[] buff = new char[128];
            void RandT()
            {
                int j = 0;
                int cmpIndex = 0;
                switch (rnd.Next(3))
                {
                    case 0: buff[j++] = '+'; ++cmpIndex; break;
                    case 2: buff[j++] = '-'; break;
                }
                int len = 10 + rnd.Next(100);

                buff[j++] = (char) ('1' + rnd.Next(9));

                for (; j < len; ++j)
                {
                    buff[j] = (char)('0' + rnd.Next(10));
                }
                string s = new string(buff, 0, j);

                T(s, s.Substring(cmpIndex));
            }

            for (int i = 0; i < 1000; ++i)
            {
                RandT();
            }
        }
    }
}
