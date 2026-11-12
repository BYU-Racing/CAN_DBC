# CAN DB File Repo for the BYU Racing Club

CANdb Studio 0.3.0 saves some DBC sections out of order, which prevents
the repository's `dbcppp` parser from loading the result. After saving in
CANdb Studio, close its editor and run this from the repository folder:

```powershell
python tools/fix_dbc_order.py
```

This reorders sections while preserving their contents. To check without
changing the file, add `--check`.

For signal properties in CANdb Studio, open the file with **CAN Database
Editor**, select the **Signals** tab, select a signal, and edit its properties.
Reopen the editor after an external file repair so it loads the current file.
