When adding method calls or field accesses from reverse engineered game structs, always add their Vtable offsets or update their field offsets in the [TitanQuestAnniversaryEdition.genny](./sdk/TitanQuestAnniversaryEdition.genny) file and then re-generate the SDK to access them from C++ code.

Reference the [regenny documentation](https://praydog.com/regenny-book/) for the regenny syntax.

Use the binary ninja MCP for reverse engineering. By default this is at http://127.0.0.1:24642/mcp
