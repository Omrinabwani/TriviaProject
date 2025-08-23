using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
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
using System.Windows.Resources;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for JoinRoomWindow.xaml
    /// </summary>
    public partial class JoinRoomWindow : Window
    {
        private DispatcherTimer roomRefreshTimer;
        public JoinRoomWindow()
        {

            InitializeComponent();
            LoadRooms();
            roomRefreshTimer = new DispatcherTimer();
            roomRefreshTimer.Interval = TimeSpan.FromSeconds(3);
            roomRefreshTimer.Tick += (s, e) => LoadRooms();
            roomRefreshTimer.Start();

        }

        protected override void OnClosed(EventArgs e) // When the window closes we stop the timer
        {
            base.OnClosed(e);
            roomRefreshTimer.Stop();
        }


        private void LoadRooms()
        {
            const byte GET_ROOMS_REQUEST_CODE = 21;

            try
            {
                string selectedRoom = (string)RoomsListBox.SelectedItem; // Save selected room

                RoomsListBox.Items.Clear();
                var stream = ConnectionManager.GetStream();
                if (stream == null || !stream.CanWrite || !stream.CanRead)
                {
                    MessageBox.Show("Connection error: Stream is invalid.");
                    return;
                }
                byte[] getRoomsRequest = new byte[5];
                getRoomsRequest[0] = GET_ROOMS_REQUEST_CODE;

                // Length 0 (4 bytes)
                getRoomsRequest[1] = 0;
                getRoomsRequest[2] = 0;
                getRoomsRequest[3] = 0;
                getRoomsRequest[4] = 0;

                stream.Write(getRoomsRequest, 0, getRoomsRequest.Length);

                int responseCode = stream.ReadByte();

                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);

                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);

                string json = Encoding.UTF8.GetString(contentBytes);
                using (JsonDocument doc = JsonDocument.Parse(json))
                {
                    JsonElement roomsArray = doc.RootElement.GetProperty("rooms");
                    int index = 0;
                    int length = roomsArray.GetArrayLength();

                    while (index < length)
                    {
                        JsonElement room = roomsArray[index];

                        int id = room.GetProperty("id").GetInt32();
                        string name = room.GetProperty("name").GetString();
                        bool isActive = room.GetProperty("isActive").GetBoolean();

                        string itemText = isActive
                            ? $"Room {id}: {name} (Game started)"
                            : $"Room {id}: {name}";

                        RoomsListBox.Items.Add(itemText);

                        if (itemText == selectedRoom) // Restore selection
                            RoomsListBox.SelectedItem = itemText;
                        index++;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading rooms: {ex.Message}");
            }
        }
        private void TryJoinRoom(int roomId)
        {
            const byte JOIN_ROOM_REQUEST_CODE = 25;

            try
            {
                NetworkStream stream = ConnectionManager.GetStream();
                if (stream == null || !stream.CanWrite)
                {
                    MessageBox.Show("No valid connection to server.");
                    return;
                }


                // Prepare the JSON request
                string json = $"{{\"roomId\": {roomId}}}";
                byte[] jsonBytes = Encoding.UTF8.GetBytes(json);
                byte[] lengthBytes = BitConverter.GetBytes(jsonBytes.Length);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);

                byte[] request = new byte[1 + 4 + jsonBytes.Length];
                request[0] = JOIN_ROOM_REQUEST_CODE;
                Array.Copy(lengthBytes, 0, request, 1, 4);
                Array.Copy(jsonBytes, 0, request, 5, jsonBytes.Length);

                stream.Write(request, 0, request.Length);

                // Read response code
                int responseCode = stream.ReadByte();

                // Read content length
                byte[] responseLengthBytes = new byte[4];
                ReadExact(stream, responseLengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(responseLengthBytes);
                int contentLength = BitConverter.ToInt32(responseLengthBytes, 0);

                // Read content
                byte[] responseBytes = new byte[contentLength];
                ReadExact(stream, responseBytes, contentLength);

                string responseJson = Encoding.UTF8.GetString(responseBytes);

                using (JsonDocument doc = JsonDocument.Parse(responseJson))
                {
                    int status = doc.RootElement.GetProperty("status").GetInt32();
                    if (status == 0) // success
                    {

                        CreatedRoomWindow roomWindow = new CreatedRoomWindow(roomId, false);
                        roomWindow.WindowState = this.WindowState;
                        roomWindow.ResizeMode = this.ResizeMode;
                        roomWindow.Show();
                        this.Close();
                    }
                    else
                    {
                        JoinRoomResponse.Text = "❌ Failed to join room. It might be full or closed.";
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error joining room: {ex.Message}");
            }
        }
        private void ReadExact(NetworkStream stream, byte[] buffer, int size)
        {
            int totalRead = 0;
            while (totalRead < size)
            {
                int bytesRead = stream.Read(buffer, totalRead, size - totalRead);
                if (bytesRead == 0)
                    throw new IOException("Disconnected while reading from the stream.");
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

        private void JoinSelectedRoom(object sender, RoutedEventArgs e)
        {
            JoinRoomResponse.Text = "";
            if (RoomsListBox.SelectedItem == null)
            {
                JoinRoomResponse.Text = "Please select a room first.";
                return;
            }
            

            string selectedItem = RoomsListBox.SelectedItem.ToString();

            int colonIndex = selectedItem.IndexOf(':');
            if (!selectedItem.StartsWith("Room ") || colonIndex == -1)
            {
                JoinRoomResponse.Text = "Invalid room format.";
                return;
            }

            if (selectedItem.Contains("(Game started)"))
            {
                JoinRoomResponse.Text = "This room has already started the game and cannot be joined\n Please wait until they finish...";
                return;
            }

            string idPart = selectedItem.Substring(5, colonIndex - 5).Trim();


            if (!int.TryParse(idPart, out int roomId))
            {
                JoinRoomResponse.Text = "Failed to extract room ID.";
                return;
            }

            TryJoinRoom(roomId);
        }

    }
}
