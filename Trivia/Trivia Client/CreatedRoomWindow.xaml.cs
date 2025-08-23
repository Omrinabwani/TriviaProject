    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Net.Sockets;
    using System.Text;
    using System.Threading.Tasks;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Data;
    using System.Windows.Documents;
    using System.Windows.Input;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    using System.Windows.Shapes;
    using System.Text.Json;
    using System.Windows.Threading;
    using Trivia_Client;

    namespace Wpf_learning
    {
        /// <summary>
        /// Interaction logic for CreatedRoomWindow.xaml
        /// </summary>
        public partial class CreatedRoomWindow : Window
        {
            private DispatcherTimer playerRefreshTimer;
            private int roomID;
            private bool isAdmin;
            private int answerTimeout;
            private int questionCount;
        public CreatedRoomWindow(int id, bool admin)
            {
                InitializeComponent();
                roomID = id;
                isAdmin = admin;
                if (isAdmin)
                {
                    CloseRoomButton.Visibility = Visibility.Visible;
                    StartGameButton.Visibility = Visibility.Visible;
                    LeaveRoomButton.Visibility = Visibility.Collapsed;
                }
                else
                {
                    CloseRoomButton.Visibility = Visibility.Collapsed;
                    StartGameButton.Visibility = Visibility.Collapsed;
                    LeaveRoomButton.Visibility = Visibility.Visible;
                }

                DisplayPlayers();
                playerRefreshTimer = new DispatcherTimer();
                playerRefreshTimer.Interval = TimeSpan.FromSeconds(0.3);
                playerRefreshTimer.Tick += (s, e) => DisplayPlayers();
                playerRefreshTimer.Start();
            
            }
            private void LeaveRoomFunction(object sender, RoutedEventArgs e)
            {
                const byte LEAVE_ROOM_REQUEST_CODE = 30;

                try
                {
                    string json = "{}";
                    byte[] request = SerializeRequest(LEAVE_ROOM_REQUEST_CODE, json);
                    NetworkStream stream = ConnectionManager.GetStream();

                    stream.Write(request, 0, request.Length);

                    int responseCode = stream.ReadByte();
                    byte[] lengthBytes = new byte[4];
                    ReadExact(stream, lengthBytes, 4);
                    if (BitConverter.IsLittleEndian)
                        Array.Reverse(lengthBytes);
                    int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                    byte[] contentBytes = new byte[contentLength];
                    ReadExact(stream, contentBytes, contentLength);
                    string content = Encoding.UTF8.GetString(contentBytes);

                    using JsonDocument doc = JsonDocument.Parse(content);
                    JsonElement root = doc.RootElement;

                    if (root.TryGetProperty("status", out JsonElement statusElement))
                    {
                        int status = statusElement.GetInt32();
                        if (status == 0) // success
                        { 
                            playerRefreshTimer.Stop();
                            var mainMenu = new MainMenuWindow();
                            mainMenu.WindowState = this.WindowState;
                            mainMenu.ResizeMode = this.ResizeMode;
                            mainMenu.Show();
                            this.Close();
                        }
                        else
                        {
                            MessageBox.Show("Failed to leave the room.");
                        }
                    }
                    else
                    {
                        MessageBox.Show("Invalid server response.");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error while leaving the room: {ex.Message}");
                }
            }

            protected override void OnClosed(EventArgs e) // When the windows closes we stop the timer
            {
                base.OnClosed(e);
                playerRefreshTimer.Stop();
            }
            private void StartGameFunction(object sender, RoutedEventArgs e)
            {
                const byte START_GAME_REQUEST_CODE = 32;
                playerRefreshTimer.Stop();
                try
                {
                    string json = "{}";
                    byte[] request = SerializeRequest(START_GAME_REQUEST_CODE, json);
                    NetworkStream stream = ConnectionManager.GetStream();

                    stream.Write(request, 0, request.Length);

                    int responseCode = stream.ReadByte();
                    byte[] lengthBytes = new byte[4];
                    ReadExact(stream, lengthBytes, 4);
                    if (BitConverter.IsLittleEndian)
                        Array.Reverse(lengthBytes);
                    int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                    byte[] contentBytes = new byte[contentLength];
                    ReadExact(stream, contentBytes, contentLength);
                    string content = Encoding.UTF8.GetString(contentBytes);

                    using JsonDocument doc = JsonDocument.Parse(content);
                    JsonElement root = doc.RootElement;

                    if (root.TryGetProperty("status", out JsonElement statusElement))
                    {
                        int status = statusElement.GetInt32();
                        if (status == 0) // success
                        {
                            var game = new GameWindow(answerTimeout, questionCount);
                            game.WindowState = this.WindowState;
                            game.ResizeMode = this.ResizeMode;
                            game.Show();
                            this.Close();
                        }
                        else
                        {
                            MessageBox.Show("Failed to close the room.");
                        }
                    }
                    else
                    {
                        MessageBox.Show("Invalid server response.");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error while closing the room: {ex.Message}");
                }
            }
            private void CloseRoomFunction(object sender, RoutedEventArgs e)
            {
                playerRefreshTimer.Stop();
                const byte CLOSE_ROOM_REQUEST_CODE = 31;
                try
                {
                    string json = "{}";
                    byte[] request = SerializeRequest(CLOSE_ROOM_REQUEST_CODE, json);
                    NetworkStream stream = ConnectionManager.GetStream();

                    stream.Write(request, 0, request.Length);

                    int responseCode = stream.ReadByte();
                    byte[] lengthBytes = new byte[4];
                    ReadExact(stream, lengthBytes, 4);
                    if (BitConverter.IsLittleEndian)
                        Array.Reverse(lengthBytes);
                    int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                    byte[] contentBytes = new byte[contentLength];
                    ReadExact(stream, contentBytes, contentLength);
                    string content = Encoding.UTF8.GetString(contentBytes);

                    using JsonDocument doc = JsonDocument.Parse(content);
                    JsonElement root = doc.RootElement;

                    if (root.TryGetProperty("status", out JsonElement statusElement))
                    {
                        int status = statusElement.GetInt32();
                        if (status == 0) // success
                        {
                            var mainMenu = new MainMenuWindow();
                            mainMenu.WindowState = this.WindowState;
                            mainMenu.ResizeMode = this.ResizeMode;
                            mainMenu.Show();
                            this.Close();
                        }
                        else
                        {
                            MessageBox.Show("Failed to close the room.");
                        }
                    }
                    else
                    {
                        MessageBox.Show("Invalid server response.");
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error while closing the room: {ex.Message}");
                }
            }
        private void MemberStartGame()
        {
            const byte MEMBER_START_GAME_REQUEST = 34;


            string Json = "{}";
            byte[] request = SerializeRequest(MEMBER_START_GAME_REQUEST, Json);
            NetworkStream stream = ConnectionManager.GetStream();

            stream.Write(request, 0, request.Length);

            int responseCode = stream.ReadByte();
            byte[] lengthBytes = new byte[4];
            ReadExact(stream, lengthBytes, 4);
            if (BitConverter.IsLittleEndian)
                Array.Reverse(lengthBytes);
            int contentLength = BitConverter.ToInt32(lengthBytes, 0);

            byte[] contentBytes = new byte[contentLength];
            ReadExact(stream, contentBytes, contentLength);

            string content = Encoding.UTF8.GetString(contentBytes);

            using JsonDocument doc = JsonDocument.Parse(content);
            JsonElement root = doc.RootElement;

            if (root.TryGetProperty("status", out JsonElement status))
            {
                if (status.GetInt32() == 0)
                {
                    var gameWindow = new GameWindow(answerTimeout, questionCount);
                    gameWindow.WindowState = this.WindowState;
                    gameWindow.ResizeMode = this.ResizeMode;
                    gameWindow.Show();
                    this.Close();
                    return;
                }
            }
        }


        private void DisplayPlayers()
        {
            const byte GET_ROOM_STATE_REQUEST_CODE = 33;

            try
            {
                string Json = "{}";
                byte[] request = SerializeRequest(GET_ROOM_STATE_REQUEST_CODE, Json);
                NetworkStream stream = ConnectionManager.GetStream();

                stream.Write(request, 0, request.Length);

                int responseCode = stream.ReadByte();
                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);

                string content = Encoding.UTF8.GetString(contentBytes);

                using JsonDocument doc = JsonDocument.Parse(content);
                JsonElement root = doc.RootElement;

                if (root.TryGetProperty("status", out JsonElement status))
                {
                    if (status.GetInt32() == 1)
                    {
                        playerRefreshTimer.Stop();
                        LeaveRoomFunction(null, null);
                        return;
                    }
                }
                if (root.TryGetProperty("answerTimeout", out JsonElement timeoutElement))
                {
                    answerTimeout = timeoutElement.GetInt32();
                }
                if (root.TryGetProperty("questionCount", out JsonElement qCountElement))
                {
                    questionCount = qCountElement.GetInt32();
                }

                if (root.TryGetProperty("hasGameBegun", out JsonElement isActive))
                {
                    if (isActive.GetBoolean() == true)
                    {
                        MemberStartGame();
                        return;
                    }

                }
                if (root.TryGetProperty("players", out JsonElement playersElement) && playersElement.ValueKind == JsonValueKind.Array)
                {
                    PlayerStackPanel.Children.Clear();

                    int length = playersElement.GetArrayLength();
                    for (int i = 0; i < length; i++)
                    {
                        string playerName = playersElement[i].GetString();

                        if (i == 0)
                        {
                            playerName += " (admin)";
                        }

                        TextBlock playerText = new TextBlock
                        {
                            Text = playerName,
                            FontSize = 16,
                            Margin = new Thickness(5)
                        };

                        PlayerStackPanel.Children.Add(playerText);
                    }

                    ServerResponse.Text = $"Found {length} players.";
                }
                else
                {
                    ServerResponse.Text = "No players found or invalid format.";
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
                        throw new IOException("Disconnected while reading from stream.");
                    totalRead += bytesRead;
                }
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
