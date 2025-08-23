using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;
using Wpf_learning;

namespace Trivia_Client
{
    public partial class GameWindow : Window
    {
        private GameResultsWindow waitingWindow;
        private Stopwatch questionTimer;
        private DispatcherTimer gameTimer;
        private DispatcherTimer gameResultTimer;
        private int answerTimeout;
        private int questionsLeft;
        private int timeLeft;
        bool isGameWaitingForPlayersOpen = false;
        public GameWindow(int answerTimeout, int numOfQuestions)
        {
            InitializeComponent();
            questionsLeft = numOfQuestions;
            QuestionsLeft.Text = questionsLeft.ToString();
            timeLeft = answerTimeout;
            this.answerTimeout = answerTimeout;
            StartGameTimer();
            StartGetGameResultTimer();
            FetchQuestionFromServer();
        }
        private void StartGetGameResultTimer()
        {
            gameResultTimer = new DispatcherTimer();
            gameResultTimer.Interval = TimeSpan.FromSeconds(1);
            gameResultTimer.Tick += GameResultTimer_Tick;
        }
        private void GameResultTimer_Tick(object sender, EventArgs e)
        {
            GetGameResultsFromServer();
        }



        private void StartGameTimer()
        {
            gameTimer = new DispatcherTimer();
            gameTimer.Interval = TimeSpan.FromSeconds(1);
            gameTimer.Tick += GameTimer_Tick;
        }

        private void GameTimer_Tick(object sender, EventArgs e)
        {
            timeLeft--;
            TimeLeft.Text = timeLeft + "s";

            if (timeLeft <= 0)
            {
                SubmitAnswer(-1); // Timeout - no answer
            }
        }

        private void AnswerClicked(object sender, RoutedEventArgs e)
        {
            Button clickedButton = sender as Button;
            int answerId = int.Parse(clickedButton.Tag.ToString());
            SubmitAnswer(answerId);
        }

        private void SubmitAnswer(int answerId)
        {
            gameTimer.Stop();

            AnswerButton1.IsEnabled = false;
            AnswerButton2.IsEnabled = false;
            AnswerButton3.IsEnabled = false;
            AnswerButton4.IsEnabled = false;

            const byte SUBMIT_ANSWER_REQUEST = 42;
            double timeTaken = questionTimer.Elapsed.TotalSeconds;
            string json = JsonSerializer.Serialize(new { answerId = answerId, answerTime = timeTaken });
            byte[] request = SerializeRequest(SUBMIT_ANSWER_REQUEST, json);

            try
            {
                NetworkStream stream = ConnectionManager.GetStream();
                stream.Write(request, 0, request.Length);

                int responseCode = stream.ReadByte();
                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int length = BitConverter.ToInt32(lengthBytes, 0);

                byte[] contentBytes = new byte[length];
                ReadExact(stream, contentBytes, length);

                questionsLeft--;
                QuestionsLeft.Text = questionsLeft.ToString();

                if (questionsLeft > 0)
                {
                    FetchQuestionFromServer();
                }
                else
                {
                    GetGameResultsFromServer();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("SubmitAnswer Error: " + ex.Message);
            }
        }

        private void FetchQuestionFromServer()
        {
            AnswerButton1.IsEnabled = true;
            AnswerButton2.IsEnabled = true;
            AnswerButton3.IsEnabled = true;
            AnswerButton4.IsEnabled = true;
            const byte GET_QUESTION_REQUEST = 40;
            string json = "{}";
            byte[] request = SerializeRequest(GET_QUESTION_REQUEST, json);
            try
            {
                NetworkStream stream = ConnectionManager.GetStream();
                stream.Write(request, 0, request.Length);

                int responseCode = stream.ReadByte();
                byte[] lengthBytes = new byte[4];
                ReadExact(stream, lengthBytes, 4);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(lengthBytes);
                int length = BitConverter.ToInt32(lengthBytes, 0);

                byte[] contentBytes = new byte[length];
                ReadExact(stream, contentBytes, length);
                string content = Encoding.UTF8.GetString(contentBytes);

                using JsonDocument doc = JsonDocument.Parse(content);
                JsonElement root = doc.RootElement;

                
                QuestionText.Text = root.GetProperty("question").GetString();
                JsonElement answers = root.GetProperty("answers");
                AnswerButton1.Content = answers.GetProperty("0").GetString();
                AnswerButton2.Content = answers.GetProperty("1").GetString();
                AnswerButton3.Content = answers.GetProperty("2").GetString();
                AnswerButton4.Content = answers.GetProperty("3").GetString();

                AnswerButton1.Tag = 1;
                AnswerButton2.Tag = 2;
                AnswerButton3.Tag = 3;
                AnswerButton4.Tag = 4;

                timeLeft = answerTimeout;
                questionTimer = Stopwatch.StartNew();
                TimeLeft.Text = $"{timeLeft}s";
                gameTimer.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show("FetchQuestion Error: " + ex.Message);
            }
        }

        private void GetGameResultsFromServer()
        {
            const byte GET_GAME_RESULT_REQUEST_CODE = 41;
            try
            {
                string json = "{}";
                byte[] request = SerializeRequest(GET_GAME_RESULT_REQUEST_CODE, json);
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

                if (root.TryGetProperty("status", out JsonElement statusElement) && statusElement.GetInt32() == 0)
                {
                    gameResultTimer.Stop();
                    if (root.TryGetProperty("results", out JsonElement resultsElement))
                    {
                        StringBuilder sb = new StringBuilder("🏁 Game Results:\n");
                        foreach (var player in resultsElement.EnumerateArray())
                        {
                            string username = player.GetProperty("username").GetString();
                            int correct = player.GetProperty("correctAnswerCount").GetInt32();
                            int wrong = player.GetProperty("wrongAnswerCount").GetInt32();
                            double avgTime = player.GetProperty("averageAnswerTime").GetDouble();
                            int score = player.GetProperty("score").GetInt32();

                            sb.AppendLine($"👤 {username} | ✅ {correct} ❌ {wrong} | ⏱️ Avg Time: {avgTime:F2}s | 🏆 Score: {score}");
                        }
                        if (waitingWindow!=null)
                        {
                            waitingWindow.Close();
                            waitingWindow = null;
                        }
                        var resultsWindow = new GameResultsWindow(sb.ToString());
                        resultsWindow.WindowState = this.WindowState;
                        resultsWindow.ResizeMode = this.ResizeMode;
                        resultsWindow.Show();
                        this.Close();
                    }
                }
                else
                {
                    if (!isGameWaitingForPlayersOpen)
                    {
                        waitingWindow = new GameResultsWindow("");
                        waitingWindow.WindowState = this.WindowState;
                        waitingWindow.ResizeMode = this.ResizeMode;
                        waitingWindow.Show();
                        this.Hide();
                        if (!gameResultTimer.IsEnabled)
                            gameResultTimer.Start();
                        isGameWaitingForPlayersOpen = true;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error getting results: " + ex.Message);
            }
        }
        private void LeaveGameFunction(object sender, RoutedEventArgs e)
        {
            const byte LEAVE_GAME_REQUEST = 43;
            gameTimer?.Stop();
            gameResultTimer?.Stop();
            try
            {
                string json = "{}";
                byte[] request = SerializeRequest(LEAVE_GAME_REQUEST, json);
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

                if (root.TryGetProperty("status", out JsonElement statusElement) && statusElement.GetInt32() == 0)
                {
                    var menu = new MainMenuWindow();
                    menu.Show();
                    this.Close();
                }
                else
                {
                    MessageBox.Show("Failed to leave the game.");
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error leaving game: " + ex.Message);
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
                Array.Reverse(lengthBytes);

            byte[] request = new byte[1 + 4 + jsonBytes.Length];
            request[0] = requestCode;
            Array.Copy(lengthBytes, 0, request, 1, 4);
            Array.Copy(jsonBytes, 0, request, 5, jsonBytes.Length);
            return request;
        }
    }
}
