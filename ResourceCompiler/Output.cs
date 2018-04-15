using System;
using System.IO;
using System.Linq;

namespace ResourceCompiler
{
    class Output : IDisposable
    {
        TextWriter header;
        TextWriter code;

        public Output(string directory, string name)
        {
            header = File.CreateText(Path.Combine(directory, Path.ChangeExtension(name, "h")));
            code = File.CreateText(Path.Combine(directory, Path.ChangeExtension(name, "c")));
        }

        public void WritePalette(string name, ushort[] data)
        {
            header.WriteLine("extern Uint16 palette_" + name + "[16];");
            code.Write("Uint16 glyphs_" + name + "[16] = { ");
            foreach (var i in Enumerable.Range(0, 16))
            {
                code.Write("0x");
                code.Write(data[i].ToString("X4"));
                code.Write(", ");
            }
            code.WriteLine("};");
        }

        public void WriteGlyphSet(string name, byte[] data)
        {
            header.WriteLine("extern Uint8 glyphs_" + name + "[8192];");
            code.Write("Uint16 glyphs_" + name + "[8192] = {");
            foreach (var i in Enumerable.Range(0, 8192))
            {
                if (i % 32 == 0)
                    code.Write("\r\n\t");

                code.Write("0x");
                code.Write(data[i].ToString("X2"));
                code.Write(", ");
            }
            code.WriteLine("\r\n};");
        }

        public void Dispose()
        {
            header.Dispose();
            code.Dispose();
        }
    }
}
