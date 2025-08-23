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
using System.Windows.Shapes;

namespace Wpf_learning
{
    /// <summary>
    /// Interaction logic for PersonalStatsWindow.xaml
    /// </summary>
    public partial class PersonalStatsWindow : Window
    {
        public PersonalStatsWindow()
        {
            byte GET_PERSONAL_STATS = 24;
            InitializeComponent();

            try
            {
                NetworkStream stream = ConnectionManager.GetStream();

                
                string json = "{}"; 
                byte[] request = SerializeRequest(GET_PERSONAL_STATS, json); 
                stream.Write(request, 0, request.Length);

                int responseCode = stream.ReadByte();

                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int contentLength = BitConverter.ToInt32(lengthBytes, 0);

                byte[] contentBytes = new byte[contentLength];
                ReadExact(stream, contentBytes, contentLength);


                string jsonResponse = Encoding.ASCII.GetString(contentBytes);

                using JsonDocument doc = JsonDocument.Parse(jsonResponse);
                JsonElement root = doc.RootElement;

                if (root.TryGetProperty("statistics", out JsonElement stats))
                {
                    double avgTime = Math.Round(stats.GetProperty("averageAnswerTime").GetDouble(), 2);
                    int correct = stats.GetProperty("correctAnswers").GetInt32();
                    int total = stats.GetProperty("totalAnswers").GetInt32();
                    int games = stats.GetProperty("gamesPlayed").GetInt32();
                    int score = stats.GetProperty("score").GetInt32();

                    StatsPanel.Children.Add(CreateStatText($"✅ Correct Answers: {correct}"));
                    StatsPanel.Children.Add(CreateStatText($"❌ Incorrect Answers: {total - correct}"));
                    StatsPanel.Children.Add(CreateStatText($"⏱️ Avg. Answer Time: {avgTime} sec"));
                    StatsPanel.Children.Add(CreateStatText($"🎮 Games Played: {games}"));
                    StatsPanel.Children.Add(CreateStatText($"⭐ Total Score: {score}"));
                }
                else
                {
                    StatsPanel.Children.Add(CreateStatText("Failed to extract stats."));
                }
            }
            catch (Exception ex)
            {
                StatsPanel.Children.Add(CreateStatText("Error: " + ex.Message));
            }
        }


        private void GetBackFunction(object sender, RoutedEventArgs e)
        {
            StatisticsWindow statisticsWindow = new StatisticsWindow();
            statisticsWindow.WindowState = this.WindowState;
            statisticsWindow.ResizeMode = this.ResizeMode;
            statisticsWindow.Show();
            this.Close();
        }



        private TextBlock CreateStatText(string text)
        {
            return new TextBlock
            {
                Text = text,
                FontSize = 16,
                Margin = new Thickness(5),
                Foreground = Brushes.White,
                FontWeight = FontWeights.Bold
            };
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
            byte[] jsonBytes = Encoding.ASCII.GetBytes(json);
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
