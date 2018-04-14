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

                using (var g = Graphics.FromImage(bitmap))
                {
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
            }

            return result;
        }

        private static ushort PackColor(Color color, bool transparent)
        {
            return (ushort)((transparent ? 0x8000 : 0x0000) | (color.R << 7) | (color.G << 2) | (color.B >> 3));
        }
    }
}
