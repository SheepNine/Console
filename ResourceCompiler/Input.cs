using System;
using System.Drawing;

namespace ResourceCompiler
{
    static class Input
    {
        public static byte[] ImportGlyphImageFile(string filename, ushort[] palette)
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
                    PackGlyph(bitmap, glyphX, glyphY, result, resultOffset, palette);
                }
            }

            return result;
        }

        static void PackGlyph(Bitmap bitmap, int glyphX, int glyphY, byte[] result, int resultOffset, ushort[] palette)
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
                        int paletteIndex = GetPaletteIndex(color, palette);

                        packedBits |= (byte)(((paletteIndex & mask) >> plane) << x);
                    }
                    result[resultOffset++] = packedBits;
                }
            }
        }

        static int GetPaletteIndex(Color color, ushort[] palette)
        {
            for (int i = 0; i < 16; i++)
            {
                if (PackColor(color, false) == (palette[i] & 0x7FFF))
                    return i;
            }
            throw new ArgumentException("Can't find palette index for color");
        }


        public static ushort[] ImportPaletteImageFile(string filename)
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

        static ushort PackColor(Color color, bool transparent)
        {
            return (ushort)((transparent ? 0x8000 : 0x0000) | (color.R << 7) | (color.G << 2) | (color.B >> 3));
        }
    }
}
