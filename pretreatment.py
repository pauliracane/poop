#! /usr//bin/env python3.5
import socket
import struct

from header import Header
from liquid import Liquid


incoming = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

incoming.bind(('', 1111))

incoming.listen(3)

while True:
    (client, addr) = incoming.accept()

    header = Header(client.recv(8))
    print("Receiving {} bytes from {}".format(header.size, addr))

    buf = client.recv(header.size - 8)
    while len(buf) < header.size - 8:
        buf += client.recv(header.size - 8 - len(buf))

    liquid = Liquid(buf)
    
    liquid.treat_fungi()
    print("Found {} after fungus".format(len(liquid.hazmats)))

    # Find out if trash; 2 for data, 1 for l, 1 for right.
    # If L or R point to # > header.size-8 / 4; scrap packet.
    # Needs to use structs.  Not sure how.  Oh well.
    for value in range(0, len(buf), 2):
        if ( buf ):
            if (value%8 == 4 or value%8 == 6):
                V = struct.unpack("!" + "H",buf[value:value+2])
                if ( V[0] > int((header.size - 8) / 8)):
                    #Send packet to downstream.  Continue at while True.
                    print("Trash Packet.")
                    trsh_out = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    trsh_out.connect(("downstream", 2222))
                    header.type = 1
                    trsh_out.send(header.serialize())
                    trsh_out.send(buf)
                    trsh_out.close()
                    client.close()
                    buf = b""
    if ( not buf):
        continue;


    liquid.treat_hg()
    print("Found {} after mercury".format(len(liquid.hazmats)))
    liquid.treat_pb()
    print("Found {} after lead".format(len(liquid.hazmats)))
    liquid.treat_se()
    print("Found {} after selenium".format(len(liquid.hazmats)))

    print("Found {} hazardous contaminants from {}".format(len(liquid.hazmats),
        addr))

    hazmat_outgoing = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    hazmat_outgoing.connect(("downstream", 8888))
    header.type = 4
    header.size = 8 + 8*len(liquid.hazmats)
    hazmat_outgoing.send(header.serialize())
    hazmat_outgoing.send(liquid.serialize_hazmat())
    hazmat_outgoing.close()

    sewage_outgoing = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sewage_outgoing.connect(("10.0.56.235", 1112))
    header.type = 0
    header.size = 8 + 8*len(liquid.data)
    sewage_outgoing.send(header.serialize())
    sewage_outgoing.send(liquid.serialize_water())
    sewage_outgoing.close()

    client.close()

