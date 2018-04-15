using System.IO;

namespace ResourceCompiler
{
    class Program
    {
        static void Main(string[] args)
        {
            string manifestFile = args[0];
            string outputDirectory = args[1];
            string outputName = args[2];
            string manifestDirectory = Path.GetDirectoryName(manifestFile);

            using (var output = new Output(outputDirectory, outputName))
            {
                ushort[] currentPalette = null;
                foreach (var line in File.ReadAllLines(manifestFile))
                {
                    if (line.StartsWith("\t"))
                    {
                        var lineBody = line.Substring(1);
                        output.WriteGlyphSet(Path.GetFileNameWithoutExtension(lineBody),
                            Input.ImportGlyphImageFile(Path.Combine(manifestDirectory, lineBody), currentPalette));
                    }
                    else
                    {
                        currentPalette = Input.ImportPaletteImageFile(Path.Combine(manifestDirectory, line));
                        output.WritePalette(Path.GetFileNameWithoutExtension(line), currentPalette);
                    }
                }
            }
        }
    }
}
