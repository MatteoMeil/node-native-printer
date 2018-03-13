using System;
using System.Threading.Tasks;

namespace windows_printer
{
    class API
    {
        public async Task<object> ListPrinters(object input)
        {
            return Methods.ListPrinters();
        }
        public async Task<object> DefaultPrinterName(object input)
        {
            return Methods.DefaultPrinterName();
        }
        public async Task<object> PrinterInfo(object printerName)
        {
            return Methods.PrinterInfo((string)(printerName));
        }
        public async Task<object> GetOptions(object printerName)
        {
            return Methods.GetOptions((string)(printerName));
        }
        public async Task<object> Print(dynamic input)
        {
            Settings settings = new Settings
            {
                Collate = (bool)input.collate,
                Duplex = (String)input.duplex,
                FromPage = (int)input.fromPage,
                ToPage = (int)input.toPage,
                Color = (bool)input.color,
                Landscape = (bool)input.landscape,
                PaperSize = (String)input.paperSize
            };

            String printerName = (String)input.printerName;
            String filePath = (String)input.filePath;
            short copies = (short)input.copies;

            return Methods.Print(printerName, settings, filePath, copies);
        }
    }
}
