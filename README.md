# node-native-printer

A node module to natively print your files

## Requirements

* ### Windows:

   * **edge** or electron-edge (depending on your environment). Both of them need to be **installed before** this module. During installation it will be detected which one is installed

* ### Unix (Mac and Linux)

   * **node-gyp** to build native modules
   * **cups** - for Linux it needs libcups2-dev and libcups2; for Mac it needs cups-devel. Both packages contains source code needed to build native modules. **It may also need this lib in production**. See [API](#api)

## Imporant notes

Due to important differences in enviroments and ecosystems between Microsoft and Unix-based systems, this module has been written in C++ for Unix and C# for Windows. This can result in different behaviours during the execution of the methods (as different execution time).

In addition, some features are only available for Unix-Based systems and will be implemented soon in Windows.

***
## Installation
```
npm install node-native-printer --save
```

## Usage
First of all you need to require the module inside your app this way:
```javascript
const printer = require('node-native-printer');
```

Then you can set a printer to work on, calling `setPrinter(printerName)`. In this way you don't need to pass else the printer name on each method it's required.

## API

### `listPrinters()`
&nbsp;&nbsp;&nbsp;&nbsp;return an array with all installed printers

### `defaultPrinterName()`
&nbsp;&nbsp;&nbsp;&nbsp;return the default printer name

### `setPrinter(printer)`
&nbsp;&nbsp;&nbsp;&nbsp;set the printer to work on


### `getCurrentPrinter()`
&nbsp;&nbsp;&nbsp;&nbsp;get current printer you are working on

### `printerInfo(printer)`

&nbsp;&nbsp;&nbsp;&nbsp;return general info about current working printer such jobs and options: 

* **printer**: printer of which get informations
* **returning value**:
   * **Windows**: return only jobs in printer queue
   * **Unix**: return jobs in printer queue and CUPS options. Theese last depends on printer

### `printerOptions(printer)`
&nbsp;&nbsp;&nbsp;&nbsp;return printer-specific options:
   * **printer**: name of the printer to print on. If not specified it will print on previously setted printer. If printer is not set it will print on default printer.
   * **Returning value**:
      * **Windows**: return an object containing main options for printer:

      ```json 
      {
          "Collate": "array containing collation options",
          "Duplexing": "array containing collation options",
          "MaxCopy": "max number of copies you can send to printer",
          "SupportsColor": "boolean indicating whether a print can print with colors",
          "PaperSheets": "available paper formats supported from printer. If custom is present it can be submitted custom width and height",
          "Resolutions": "printer resolutions (i.e.: High, Medium)"
      }
      ```

      * **Unix**: return an object containing printer-specific options and from PPD file.

### `print(filename[, options, printer])`
   * **filename**: file to print (**N.B.**: windows supports only PDF)
   * **options**: a JSON object containing options for printer:
      * **Windows**: default options:
         ```json
         {
             "collate": true,
             "color": true,
             "copies": 1,
             "duplex": "Default",
             "landscape": false,
             "paperSize": "",
             "fromPage": 0,
             "toPage": 0
         }
         ```
         **Notes:** duplex is case sensitive, so be careful to write correctly. `"paperSize"` refers to size of sheet to print on; if you want to print on a paper with custom dimensions, pass `"Custom.WidthxHeight"` where Width and Height are dimensions in hundredths of an inch. `"fromPage": 0` means document will be printed from first page; `"toPage": 0` means document will be printed from `"fromPage"` to last page.

      * **Unix**: you can use [command-line options](https://www.cups.org/doc/options.html) in JSON-style and/or printer-specific options retrieved from `printerOptions()`; i.e.:

         ```json      
         {
             "landscape": true,
             "n": 2,
             "sides": "two-sided-long-edge"
         }
         ```

         For options that doesn't have a value (like `landscape` or `fit-to-page`) you can assign a boolean (see above)

         It will be generated and executed a command like `lp -d printerName /path/to/filename -o landscape -n 2 -o sides=two-sided-long-edge`
   * **Returning value (only for Unix)**: job id of work sent to printer
