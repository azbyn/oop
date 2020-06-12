namespace big_int_interface
{
    public readonly struct Token
    {
        public enum Type
        {
            Eof,//only to be used inside the parser
            Int,        // duh
            Var,        // identifier like sqrt...
            LeftParen,  // (
            RightParen, // (
            Lambda,     // λ or \
            Dot,        // . (for λ a. a)
            Plus,       // +
            Minus,      // -
            Div,        // /
            Mul,        // *
            Mod,        // %
            Pow,        // ^
            Eq,         // ==
            Ne,         // !=
            Lt,         // <=
            Gt,         // >=
            Le,         // <
            Ge,         // >
            And,        // &&
            Or,         // ||
            Not,        // !
            Xor,        // ^^
            BAnd,       // &
            BOr,        // |
            Lsh,        // <<
            Rsh,        // >>
            Assign,     // =
            Question,   // ?
            Colon,      // :
        }
        public readonly Type type;
        public readonly string val;
        public Token(Type type, string val)
        {
            this.type = type;
            this.val = val;
        }
        public override string ToString()
        {
            return $"{{tok {type} '{val}'}}";
        }
    }
}
