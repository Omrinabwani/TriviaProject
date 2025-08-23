using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Wpf_learning
{
    internal class JsonRequestPacketSerializer
    {

        static public byte[] SerializeRequest(byte requestCode, string json)
        {
            byte[] jsonBytes = Encoding.ASCII.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes);

            byte[] request = new byte[1 + 4 + jsonBytes.Length];
            request[0] = requestCode;
            Array.Copy(lengthBytes, 0, request, 1, 4);
            Array.Copy(jsonBytes, 0, request, 5, jsonBytes.Length);

            return request;
        }
    }
}
