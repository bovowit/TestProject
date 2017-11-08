using iTextSharp.text;

namespace SeanReport
{
    internal class HeaderFooter
    {
        private Phrase phrase;
        private bool v;

        public HeaderFooter(Phrase phrase, bool v)
        {
            this.phrase = phrase;
            this.v = v;
        }

        public int Border { get; internal set; }
    }
}