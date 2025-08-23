using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Reflection.Metadata;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for CreateRoomWindow.xaml
    /// </summary>
    public partial class CreateRoomWindow : Window
    {
        public CreateRoomWindow()
        {
            InitializeComponent();
        }
        private void CreateRoomFunction(object sender, RoutedEventArgs e)
        {
            const byte CREATEROOM_REQUEST_CODE = 26;

            try
            {
                string roomName = RoomNameInput.Text.Trim();

                if (string.IsNullOrEmpty(roomName))
                {
                    ServerResponse.Text = "Room name cannot be empty.";
                    return;
                }

                int numOfPlayers = int.Parse(NumOfPlayersInput.Text);
                int numOfQuestions = int.Parse(NumOfQuestionsInput.Text);
                int time = int.Parse(TimeInput.Text);

                if (numOfPlayers < 1 || numOfPlayers > 10)
                {
                    ServerResponse.Text = "Number of players must be between 1 and 10.";
                    return;
                }
                if (numOfQuestions < 1 || numOfQuestions > 30)
                {
                    ServerResponse.Text = "Number of questions must be between 1 and 30.";
                    return;
                }
                if (time < 2 || time > 100)
                {
                    ServerResponse.Text = "Answer time must be between 2 and 100 seconds.";
                    return;
                }
                string Json = $"{{\"roomName\": \"{roomName}\", \"maxUsers\": {numOfPlayers}, \"questionCount\": {numOfQuestions}, \"answerTimeout\": {time}}}";
                byte[] request = SerializeRequest(CREATEROOM_REQUEST_CODE, Json);
                Console.WriteLine(Json);

                // Connect to server
                NetworkStream stream = ConnectionManager.GetStream();
                if (stream == null || !stream.CanWrite)
                {
                    ServerResponse.Text = "No valid connection to server.";
                    return;
                }

                // Send request
                stream.Write(request, 0, request.Length);

                // Read 1 byte response code
                int responseCode = stream.ReadByte();

                // Read 4-byte response length
                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                // Read response content
                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);
                string content = Encoding.UTF8.GetString(contentBytes);
                    int roomID = -1;
                    using (JsonDocument doc = JsonDocument.Parse(content))
                    {
                    Console.WriteLine($"Raw response: {content}");
                    int status = doc.RootElement.GetProperty("status").GetInt32();
                        if (status == 0)
                        {
                            roomID = doc.RootElement.GetProperty("roomID").GetInt32();

                            CreatedRoomWindow createdRoomWindow = new CreatedRoomWindow(roomID, true);
                            createdRoomWindow.WindowState = this.WindowState;
                            createdRoomWindow.ResizeMode = this.ResizeMode;
                            createdRoomWindow.Show();
                            this.Close();
                        }
                        else
                        {
                            ServerResponse.Text = "Another room has the same name.\nPlease choose a different name and try again...";
                        }
                    }
            }
            catch (SocketException)
            {
                ServerResponse.Text = "Connection error: Unable to reach the server.";
            }
            catch (IOException)
            {
                ServerResponse.Text = "IO error: Connection lost during communication.";
            }
            catch (Exception ex)
            {
                ServerResponse.Text = $"Unexpected error: {ex.Message}";
            }
        }
        private void ReadExact(NetworkStream stream, byte[] buffer, int size)
        {
            int totalRead = 0;
            while (totalRead < size)
            {
                int bytesRead = stream.Read(buffer, totalRead, size - totalRead);
                if (bytesRead == 0)
                    throw new IOException("Disconnected while reading data from stream.");
                totalRead += bytesRead;
            }
        }

        private void GetBackFunction(object sender, RoutedEventArgs e)
        {
            MainMenuWindow mainMenuWindow = new MainMenuWindow();
            mainMenuWindow.WindowState = this.WindowState;
            mainMenuWindow.ResizeMode = this.ResizeMode;
            mainMenuWindow.Show();
            this.Close();
        }
        private byte[] SerializeRequest(byte requestCode, string json)
        {
            byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
            byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);

            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes); // Convert to big-endian

            byte[] request = new byte[1 + 4 + jsonBytes.Length];
            request[0] = requestCode;
            Array.Copy(lengthBytes, 0, request, 1, 4);
            Array.Copy(jsonBytes, 0, request, 5, jsonBytes.Length);
            return request;
        }
    }
 }
