using System.Configuration;
using System.Data;
using System.IO;
using System.Net.Sockets;
using System.Windows;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {

    }

    public static class ConnectionManager
    {
        private static readonly string serverIp = "127.0.0.1";
        private static readonly int port = 55551;

        private static TcpClient client;
        private static NetworkStream stream;

        public static void Connect()
        {
            if (client == null || !client.Connected)
            {
                client = new TcpClient(serverIp, port);
                stream = client.GetStream();
            }
        }
        public static TcpClient GetClient() => client;
        public static NetworkStream GetStream() => stream;
    }


}
