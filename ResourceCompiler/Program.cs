using System;
using System.Drawing;
using System.IO;

namespace ResourceCompiler
{
    class Program
    {
        static void Main(string[] args)
        {
            string filename = args[0];

            ushort[] packedColors = ImportPaletteImageFile(filename);

            Console.Write("Uint16 palette_" + Path.GetFileNameWithoutExtension(filename) + "[16] = { ");
            foreach (ushort packedColor in packedColors)
                Console.Write("0x" + packedColor.ToString("X4") + ", ");
            Console.WriteLine("};");

            byte[] packedGlyphBytes = ImportGlyphImageFile(args[1], packedColors);

            Console.Write("Uint8 glpyh_" + Path.GetFileNameWithoutExtension(args[1]) + "[8192] = {");
            for (int i = 0; i < 8192; i++)
            {
                if (i % 32 == 0)
                    Console.Write("\r\n\t");
                Console.Write("0x" + packedGlyphBytes[i].ToString("X2") + ", ");
            }
            Console.WriteLine("\r\n};");
        }

        private static byte[] ImportGlyphImageFile(string filename, ushort[] packedColors)
        {
            byte[] result = new byte[8192];

            using (Bitmap bitmap = (Bitmap)Image.FromFile(filename))
            {
                if (bitmap.Width != 64 || bitmap.Height != 256)
                    throw new ArgumentException("Unexpected bitmap image dimensions");

                for (var i = 0; i < 256; i++)
                {
                    var glyphX = i % 8;
                    var glyphY = i / 8;
                    var resultOffset = 32 * i;
                    PackGlyph(bitmap, glyphX, glyphY, result, resultOffset, packedColors);
                }
            }

            return result;
        }

        private static void PackGlyph(Bitmap bitmap, int glyphX, int glyphY, byte[] result, int resultOffset, ushort[] packedColors)
        {
            for (int plane = 0; plane < 4; plane++)
            {
                int mask = 1 << plane;
                for (int y = 0; y < 8; y++)
                {
                    byte packedBits = 0;
                    for (int x = 0; x < 8; x++)
                    {
                        Color color = bitmap.GetPixel(glyphX * 8 + (7 - x), glyphY * 8 + y);
                        int paletteIndex = GetPaletteIndex(color, packedColors);

                        packedBits |= (byte)(((paletteIndex & mask) >> plane) << x);
                    }
                    result[resultOffset++] = packedBits;
                }
            }
        }

        private static int GetPaletteIndex(Color color, ushort[] packedColors)
        {
            for (int i = 0; i < 16; i++)
            {
                if (PackColor(color, false) == (packedColors[i] & 0x7FFF))
                    return i;
            }
            throw new ArgumentException("Can't find palette index for color");
        }

        private static ushort[] ImportPaletteImageFile(string filename)
        {
            ushort[] result = new ushort[16];
            Color OPAQUE = Color.FromArgb(0, 0, 0);
            Color TRANSPARENT = Color.FromArgb(255, 255, 255);

            using (Bitmap bitmap = (Bitmap)Image.FromFile(filename))
            {
                if (bitmap.Width != 16 || bitmap.Height != 2)
                    throw new ArgumentException("Unexpected bitmap image dimensions");

                for (int i = 0; i < 16; i++)
                {
                    Color color = bitmap.GetPixel(i, 0);
                    if (color.A != 255 || (color.R & 0x7) != 0 || (color.G & 0x7) != 0 || (color.B & 0x7) != 0)
                        throw new ArgumentException("Unexpected color value at index " + i);

                    Color transparentFlag = bitmap.GetPixel(i, 1);
                    if (transparentFlag != OPAQUE && transparentFlag != TRANSPARENT)
                        throw new ArgumentException("Unexpected transparent flag value at index " + i);

                    result[i] = PackColor(color, transparentFlag == TRANSPARENT);
                }
            }

            return result;
        }

        private static ushort PackColor(Color color, bool transparent)
        {
            return (ushort)((transparent ? 0x8000 : 0x0000) | (color.R << 7) | (color.G << 2) | (color.B >> 3));
        }
    }
}
