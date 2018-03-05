using System;
using System.Linq;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing.Printing;
using System.Printing;
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
        public static PrintSystemJobInfo[] PrinterInfo(String printer)
        {
            PrinterSettings printerSettings = new PrinterSettings { PrinterName = printer };

            if (!printerSettings.IsValid)
                printer = (new PrinterSettings()).PrinterName; //fallback to default printer

            PrintJobInfoCollection jobs = (new PrintServer())
                                          .GetPrintQueue(printer)
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

            PrintCapabilities pc = new PrintServer()
                                    .GetPrintQueue(printerName)
                                    .GetPrintCapabilities();

            List<string> temp = new List<string>();

            foreach (PaperSize paperSize in ps.PaperSizes)
            {
                temp.Add(paperSize.PaperName);
            }

            string[] paperSheets = temp.ToArray();
            temp.Clear();

            foreach (PrinterResolution resolution in ps.PrinterResolutions)
            {
                temp.Add(resolution.Kind.ToString());
            }

            string[] resolutions = temp.ToArray();

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
        public static bool PrintPDF(String printer, Settings settings, String filename, short copies)
        {
            PrinterSettings printerSettings = new PrinterSettings { PrinterName = printer };

            if (!printerSettings.IsValid)
                printerSettings = new PrinterSettings(); //fallback to default printer

            printerSettings.Copies = copies;
            printerSettings.Collate = settings.Collate;

            Duplex d;

            if (Enum.TryParse<Duplex>(settings.Duplex, out d))
                printerSettings.Duplex = d;

            if (settings.FromPage > 0)
                printerSettings.FromPage = settings.FromPage;

            if (settings.ToPage > 0)
                printerSettings.ToPage = settings.ToPage;

            PageSettings pageSettings = new PageSettings(printerSettings)
            {
                Margins = new Margins(0, 0, 0, 0)
            };

            string paperSize = settings.PaperSize;
            if (paperSize.Length > 0)
            {
                if (paperSize.Contains("Custom"))
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
                
            try
            {
                using (var document = PdfDocument.Load(filename))
                {
                    using (PrintDocument printDocument = document.CreatePrintDocument())
                    {
                        printDocument.PrinterSettings = printerSettings;
                        printDocument.DefaultPageSettings = pageSettings;
                        printDocument.PrintController = new StandardPrintController();
                        printDocument.QueryPageSettings += delegate (object sender, QueryPageSettingsEventArgs e) {
                            e.PageSettings.Landscape = settings.Landscape;
                            e.PageSettings.Color = settings.Color;
                        };
                        printDocument.Print();
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
        #endregion
    }
}