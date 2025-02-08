Client Bridge

A bridge allowing you to connect to a gateway over the network.

Current implementation uses Websocket for easy integration with any programming language.

Different protocols will be supported

* JSON (text)
* Flatbuffers (binary)
* SBE (binary)

Schema definitions can be found [here](https://github.com/roq-trading/roq-schema)

In particular, the Flatbuffers and SBE binary protocols are easy to integrate with any programming language.
(You will need to auto-generate encoding/decoding code using the schema definitions and the tools coming with Flatbuffers or SBE.)
