using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HangmanInterface
{
    public interface IWordGenerator
    {
        string GenWord();
    }

    public class FileWordGenerator : IWordGenerator
    {
        readonly Random rand;
        readonly string[] words;

        public FileWordGenerator(string[] words)
        {
            this.words = words;
            rand = new Random();
        }

        public string GenWord() => words[rand.Next(words.Length)];
    }
}
