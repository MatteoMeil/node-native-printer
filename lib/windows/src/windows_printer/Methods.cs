using System;
using System.Linq;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
using System.Drawing.Printing;
using System.Printing;
using System.Web;

using PdfiumViewer;

namespace windows_printer
{
    public struct Settings
    {
        //PrinterSettings
        public bool Collate;
        public String Duplex;
        public int FromPage;
        public int ToPage;

        //PageSettings
        public bool Color;
        public bool Landscape;
        public String PaperSize;
    }
    public struct PrinterOptions
    {
        public string[] Collate;
        public string[] Duplexing;
        public short MaxCopy;
        public bool SupportsColor;
        public string[] PaperSheets;
        public string[] Resolutions;
    }

    public class Methods
    {
        #region PUBLIC_METHODS
        public static String[] ListPrinters()
        {
            PrinterSettings.StringCollection printers = System.Drawing.Printing.PrinterSettings.InstalledPrinters;

            String[] result = new String[printers.Count];

            int i = 0;
            foreach (string printer in printers)
            {
                result[i++] = printer;
            }

            return result;
        }
        public static String DefaultPrinterName()
        {
            PrinterSettings defaultPrinter = new PrinterSettings();
            return defaultPrinter.PrinterName;
        }
        public static PrintSystemJobInfo[] PrinterInfo(String printerName)
        {
            PrinterSettings printerSettings = new PrinterSettings { PrinterName = printerName };

            if (!printerSettings.IsValid)
                printerName = (new PrinterSettings()).PrinterName; //fallback to default printer

            PrintJobInfoCollection jobs = Methods.GetPrintQueue(printerName)
                                          .GetPrintJobInfoCollection();

            return jobs.ToArray();
        }
        public static PrinterOptions GetOptions(string printerName)
        {
            PrinterSettings ps = new PrinterSettings() { PrinterName = printerName };

            if (!ps.IsValid)
            {
                ps = new PrinterSettings();
                printerName = ps.PrinterName;
            }

            PrintCapabilities pc = Methods.GetPrintQueue(printerName)
                                   .GetPrintCapabilities();

            string[] paperSheets = new string[ps.PaperSizes.Count];

            for (int i = 0; i < ps.PaperSizes.Count; i++)
            {
                paperSheets[i] = ps.PaperSizes[i].PaperName;
            }

            string[] resolutions = new string[ps.PrinterResolutions.Count];

            for (int i = 0; i < ps.PrinterResolutions.Count; i++)
            {
                resolutions[i] = ps.PrinterResolutions[i].Kind.ToString();
            }

            return new PrinterOptions
            {
                Collate = setInfo(pc.CollationCapability),
                Duplexing = setInfo(pc.DuplexingCapability),
                MaxCopy = (short)pc.MaxCopyCount,
                SupportsColor = ps.SupportsColor,
                PaperSheets = paperSheets,
                Resolutions = resolutions
            };
        }
        public static bool Print(String printer, Settings settings, String filename, short copies)
        {
            PrinterSettings printerSettings = new PrinterSettings { PrinterName = printer };

            if (!printerSettings.IsValid)
            {
                printerSettings = new PrinterSettings(); //fallback to default printer
            }

            printerSettings.Copies = copies;
            printerSettings.Collate = settings.Collate;


            if (Enum.TryParse<Duplex>(settings.Duplex, out Duplex d))
                printerSettings.Duplex = d;

            if (settings.FromPage > 0)
                printerSettings.FromPage = settings.FromPage;

            if (settings.ToPage > 0)
                printerSettings.ToPage = settings.ToPage;


            PageSettings pageSettings = new PageSettings(printerSettings)
            {
                Margins = new Margins(0, 0, 0, 0),
                Color = settings.Color,
                Landscape = settings.Landscape
            };

            string paperSize = settings.PaperSize;
            if (paperSize.Length > 0)
            {
                if (paperSize.Contains("Custom") || paperSize.Contains("custom"))
                {
                    int dot = paperSize.IndexOf('.');
                    int x = paperSize.IndexOf('x');
                    int width = Int32.Parse(paperSize.Substring(dot + 1, x - dot - 1));
                    int height = Int32.Parse(paperSize.Substring(x + 1));
                    pageSettings.PaperSize = new PaperSize("Custom", width, height);
                }
                else
                {
                    foreach (PaperSize ps in printerSettings.PaperSizes)
                    {
                        if (ps.PaperName == paperSize)
                        {
                            pageSettings.PaperSize = ps;
                            break;
                        }
                    }

                }
            }

            string mimeType = MimeMapping.GetMimeMapping(filename);
            string[] octetStreamSupportedExtensions = { ".c++", ".cc", ".com", ".conf", ".hh", ".java", ".log" };

            switch (mimeType)
            {
                case "application/pdf":
                    return PrintPDF(filename, printerSettings, pageSettings, copies);

                case "application/octet-stream":
                    string extension = filename.Substring(filename.LastIndexOf("."));

                    if (octetStreamSupportedExtensions.Contains(extension))
                        return PrintText(filename, printerSettings, pageSettings, copies);
                    else
                        return false;

                case "application/x-javascript":
                    return PrintText(filename, printerSettings, pageSettings, copies);

                default:
                    if (mimeType.Contains("image/"))
                        return PrintImage(filename, printerSettings, pageSettings, copies);
                    else if (mimeType.Contains("text/"))
                        return PrintText(filename, printerSettings, pageSettings, copies);
                    else
                        return false;
            }

            return false;
        }
        #endregion

        #region PRIVATE_METHODS
        private static string[] setInfo<T>(ReadOnlyCollection<T> collection)
        {
            List<string> temp = new List<string>();
            foreach (T value in collection)
            {
                temp.Add(value.ToString());
            }

            return temp.ToArray();
        }

        private static PrintQueue GetPrintQueue(String printerName)
        {
            if (printerName.Substring(0, 2) == @"\\")
            {
                int index = printerName.IndexOf('\\', 2);
                if (index > 0)
                {
                    String host = printerName.Substring(0, index);
                    String printer = printerName.Substring(index + 1);
                    return (new PrintServer(host)).GetPrintQueue(printer);
                }
            }

            return (new PrintServer()).GetPrintQueue(printerName);
        }

        private static bool PrintPDF(string filename, PrinterSettings printerSettings, PageSettings pageSettings, int copies)
        {
            bool landscape = pageSettings.Landscape,
                 color = pageSettings.Color;

            try
            {
                using (var document = PdfDocument.Load(filename))
                {
                    using (PrintDocument pd = document.CreatePrintDocument())
                    {
                        pd.PrinterSettings = printerSettings;
                        pd.DefaultPageSettings = pageSettings;
                        pd.PrintController = new StandardPrintController();
                        pd.QueryPageSettings += delegate (object sender, QueryPageSettingsEventArgs e) {
                            e.PageSettings.Landscape = landscape;
                            e.PageSettings.Color = color;
                        };
                        pd.Print();
                    }
                }

                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }
        }
        private static bool PrintImage(string filename, PrinterSettings printerSettings, PageSettings pageSettings, int copies)
        {
            bool landscape = pageSettings.Landscape,
                 color = pageSettings.Color;
            try
            {
                PrintDocument pd = new PrintDocument
                {
                    PrinterSettings = printerSettings,
                    DefaultPageSettings = pageSettings
                };

                pd.PrintPage += delegate (object sender, PrintPageEventArgs args)
                {
                    Image image = Image.FromFile(filename);
                    if (landscape)
                    {
                        // in landscape mode width is actually height and vice-versa
                        var width = args.PageSettings.PrintableArea.Width;
                        var height = args.PageSettings.PrintableArea.Height;
                        args.Graphics.DrawImage(image, 0, 0, height, width);
                    }
                    else
                    {
                        var width = args.PageSettings.PrintableArea.Width;
                        var height = args.PageSettings.PrintableArea.Height;
                        args.Graphics.DrawImage(image, 0, 0, width, height);
                    }
                };

                pd.QueryPageSettings += delegate (object sender, QueryPageSettingsEventArgs e) {
                    e.PageSettings.Landscape = landscape;
                    e.PageSettings.Color = color;
                };
                pd.Print();

                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }
        }
        private static bool PrintText(string filename, PrinterSettings printerSettings, PageSettings pageSettings, int copies)
        {
            bool landscape = pageSettings.Landscape,
                 color = pageSettings.Color;

            string text = System.IO.File.ReadAllText(filename);

            try
            {
                PrintDocument pd = new PrintDocument
                {
                    PrinterSettings = printerSettings,
                    DefaultPageSettings = pageSettings
                };

                pd.PrintPage += delegate (object sender, PrintPageEventArgs args)
                {
                    args.Graphics.DrawString(
                        text,
                        new Font("Times New Roman", 12),
                        new SolidBrush(Color.Black),
                        args.MarginBounds
                    );
                };

                pd.QueryPageSettings += delegate (object sender, QueryPageSettingsEventArgs e)
                {
                    e.PageSettings.Landscape = landscape;
                    e.PageSettings.Color = color;
                };
                pd.Print();

                return true;
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return false;
            }
        }
        #endregion
    }
}