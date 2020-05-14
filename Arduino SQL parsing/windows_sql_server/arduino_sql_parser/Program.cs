using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.IO;                                                                                                                                                                
using System.Data.SqlClient;

//

namespace arduino_sql_parser
{
    class Program
    {
        static int connection_count = 0;
        

        static void Main(string[] args)
        {
            Console.WriteLine("...");

            

            //Console.WriteLine(write_query(connection_string, "INSERT INTO Data (id,name,email) VALUES (NULL, 'yoyo', 'nono@g.com');"));

            //Console.WriteLine(read_query(connection_string, "SELECT * FROM data ORDER BY id"));

            //Console.ReadLine();

            Console.WriteLine("Hello");

            TcpListener listener = new TcpListener(IPAddress.Any, 6789);
            TcpClient client;

            listener.Start();

            Console.WriteLine("Awaiting connection...");
            while (true)
            {
                client = listener.AcceptTcpClient();
                client.ReceiveTimeout = 20000;
                ThreadPool.QueueUserWorkItem(ThreadProc, client);
            }
        }


        private static void ThreadProc(object obj)
        {
            TcpClient client = (TcpClient)obj;
            Byte[] bytes = new Byte[256];
            String data = null;

            connection_count++;
            Console.WriteLine("Connection accepted!\t#" + connection_count);
            Console.WriteLine(((IPEndPoint)client.Client.RemoteEndPoint).Address.ToString());

            NetworkStream stream = client.GetStream();
            stream.ReadTimeout = 10000;
            string tosend = "HELO\r";
            byte[] msg_start = System.Text.Encoding.ASCII.GetBytes(tosend);
            stream.Write(msg_start, 0, msg_start.Length);
            string ss = "";
            int i;


            try
            {
                while ((i = stream.Read(bytes, 0, bytes.Length)) != 0)
                {
                    // Translate data bytes to a ASCII string.
                    data = System.Text.Encoding.ASCII.GetString(bytes, 0, i).Trim();
                    if (data.Length > 0)
                    {
                        Console.WriteLine("Received: {0}", data);
                        ss += data;
                        data = data.ToUpper();

                        if(ss.IndexOf(">>")>=0)
                        {
                            ss = ss.Substring(ss.IndexOf(">>"));
                            if(ss.IndexOf("<<")>0)
                            {
                                ss = ss.Substring(0, ss.IndexOf("<<") + 2);
                                Console.WriteLine("CMD: " + ss);
                                tosend = process_parse_cmd(ss);

                                tosend += "\nEND\n";
                                byte[] msg = System.Text.Encoding.ASCII.GetBytes(tosend);
                                stream.Write(msg, 0, msg.Length);
                                Console.WriteLine("Sent:\n{0}", tosend);
                                break;
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {

                Console.WriteLine("Failed to read request: ");
                Console.WriteLine(ex.Message);
            }
            finally
            {
                client.Close();
            }


            Console.WriteLine("Connection Ends");

        }

        static string process_parse_cmd(string sql_querry)
        {
            /*string connection_string =
            "Data Source=LIGHTNING\\SQLEXPRESS;" +
            "Initial Catalog=arduino;" +
            "User id=sa;" +
            "Password=897898;";
            */
            string path = "connection.ini";

            if (!File.Exists(path))
            {
                return "No SQL Config File\n";
            }
            

            string connection_string = File.ReadAllText(path).Trim();
            Console.WriteLine(connection_string);

            string ssq = sql_querry;
            if(ssq.IndexOf("R##(")>=0)
            {
                ssq = ssq.Substring(ssq.IndexOf("R##(") + 4);
                if(ssq.IndexOf(")##")>0)
                {
                    ssq = ssq.Substring(0, ssq.IndexOf(")##"));
                    Console.WriteLine("SQL Read Command: ");
                    Console.WriteLine(ssq);
                    return read_query(connection_string, ssq);
                }
            }
            else
            {
                ssq = ssq.Substring(ssq.IndexOf("W##(") + 4);
                if (ssq.IndexOf(")##") > 0)
                {
                    ssq = ssq.Substring(0, ssq.IndexOf(")##"));
                    Console.WriteLine("SQL Write Command: ");
                    Console.WriteLine(ssq);
                    return write_query(connection_string, ssq);
                }

            }
            return "Invalid Command\n";
        }

        static string read_query(string conString, string sql_cmd)
        {
            string result = "<EMPTY>";
            using (SqlConnection connection = new SqlConnection(conString))
            {
                connection.Open();
                try
                {
                    using (SqlCommand command = new SqlCommand(sql_cmd, connection))
                    {
                        using (SqlDataReader reader = command.ExecuteReader())
                        {
                            result = "<table>\n";
                            while (reader.Read())
                            {
                                for (int i = 0; i < reader.FieldCount; i++)
                                {
                                    result += reader.GetValue(i).ToString();
                                    result += "\t";
                                }
                                result += "\n";
                            }
                            result += "</table>\n";
                        }
                    }
                }
                catch (Exception ex)
                {
                    result = "SQL ERROR\n";
                    result += ex.Message;
                }
            }
            return result;

        }


        static string write_query(string conString, string sql_cmd)
        {
            string result = "<EMPTY>";

            try
            {
                using (SqlConnection connection = new SqlConnection(conString))
                {
                    SqlCommand cmd = new SqlCommand();

                    cmd.CommandText = sql_cmd;
                    //cmd.CommandType = CommandType.Text;
                    cmd.Connection = connection;

                    connection.Open();
                    int res = cmd.ExecuteNonQuery();
                    result = "RESULT=";
                    result += res.ToString();
                    result += "\n";
                }
            }
            catch(Exception ex)
            {
                result = "SQL ERROR\n";
                result += ex.Message;
            }
            return result;
        }
    }
}
