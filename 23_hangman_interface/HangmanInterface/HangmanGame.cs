using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HangmanInterface
{
    public interface IGraphicsDrawer 
    {
        void ResetScreen();
        void DrawHangedMan(int state, int numTries);
        void DrawDisplayWord(char[] display);
        void DrawWrongLetters(List<char> letters);
        //returns true if it should continue
        bool EndScreen(bool won);
    }
    public class HangmanGame
    {
        // the word being guessed (eg potato)
        string word;
        // the current word being displayed (eg _ot_t_o)
        char[] display;
        // the wrong letter guesses (eg jw)
        readonly List<char> wrong = new List<char>();

        public int NumTries { get; private set; }

        readonly IWordGenerator gen;
        readonly IGraphicsDrawer graphics;

        public HangmanGame(IWordGenerator gen, IGraphicsDrawer graphics, int numTries = 8)
        {
            this.gen = gen;
            this.graphics = graphics;
            this.NumTries = numTries;
            InitWord();
        }

        void InitWord(string word)
        {
            wrong.Clear();
            this.word = word;
            this.display = Enumerable.Repeat('_', word.Length).ToArray();
            //Console.WriteLine($"word: {word}");
            UpdateGraphics();
        }
        public void CheckLetter(char l)
        {
            if (wrong.Contains(l)) return;
            if (display.Contains(l)) return;
            var pos = word.IndexOf(l);

            if (pos < 0)
            {
                wrong.Add(l);
                CheckEndOfGame();
                UpdateGraphics();
                return;
            }

            do
            {
                display[pos] = l;
                pos = word.IndexOf(l, pos + 1);

            } while (pos >= 0);
            UpdateGraphics();
            CheckWin();
        }
        void CheckEndOfGame()
        {
            if (wrong.Count >= NumTries)
            {
                display = word.ToCharArray();
                Gameover(false);
            }
        }
        void CheckWin()
        {
            if (!display.Contains('_'))
                Gameover(true);
        }
        void Gameover(bool won)
        {
            UpdateGraphics();
            if (graphics.EndScreen(won))
                Reset();
        }

        void UpdateGraphics()
        {
            graphics.DrawHangedMan(wrong.Count, NumTries);
            graphics.DrawWrongLetters(wrong);
            graphics.DrawDisplayWord(display);
        }
        void InitWord() => InitWord(gen.GenWord());

        void Reset()
        {
            graphics.ResetScreen();
            InitWord();
        }
    }
}
