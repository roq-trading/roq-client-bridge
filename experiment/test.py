#!/usr/bin/env python

import codecs

from websockets.sync.client import connect

from google.protobuf.timestamp_pb2 import Timestamp

import api_pb2 as roq

hex_encoder = codecs.getencoder("hex")

source_info = roq.SourceInfo()

create_order = roq.CreateOrder(
    exchange="deribit",
    symbol="BTC-PERPETUAL",
)

# event = roq.Event(source_info=source_info, create_order=create_order)
event = roq.Event(create_order=create_order)

request = event.SerializeToString()
print(hex_encoder(request)[0])

with connect("ws://localhost:1234?user=trader&codec=protobuf") as ws:
    ws.send(request)
    while True:
        message = ws.recv()
        print(hex_encoder(message)[0])
#        server_msg = order_execution_models.ServerMsg().ParseFromString(message)
#        print(server_msg)
#        # print(google.protobuf.text_format.MessageToString(server_msg))
