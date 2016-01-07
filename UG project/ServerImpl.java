import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author student
 */
public class ServerImpl {
    public static void main(String args[]) throws Exception {
    ServerSocket ss;
	Socket s;
	//ServerImpl() {
		//try {
			ss = new ServerSocket(3070);
              while(true)
            {
                s = ss.accept();
                Server se = new Server(s);
            }
			
}
}
class Server extends Thread {
    Socket s,s1;
	InetAddress ia;
	String toSend;
	InputStream is;
	OutputStream os;
	DataInputStream dis;
	DataOutputStream dos;
	String arr[] = new String[3];
	int cport;

     public Server(Socket s2)
    {
          s=s2;
          this.start();
    }
    @Override
     public void run() {
        try {
            ia = InetAddress.getLocalHost();
            toSend = ia.getHostAddress().concat(" ").concat("3070");
            //} catch(Exception e) {
            //}
            //}
//	}
            //void readRequest() throws Exception {

            is = s.getInputStream();
            dis = new DataInputStream(is);
            String recvd = dis.readUTF();
            System.out.println(recvd);
            arr = recvd.split(" ");
            cport = Integer.parseInt(arr[2]);
            //}
            // void serveRequest() throws Exception {
            s1 = new Socket(arr[1], cport);
            System.out.println(arr[1]);
            System.out.println(cport);
            os = s1.getOutputStream();

            FileInputStream fis = new FileInputStream(arr[0]);
            File myFile = new File(arr[0]);
            BufferedInputStream bis = new BufferedInputStream(fis);
            byte[] by = new byte[(int) myFile.length()];
            bis.read(by, 0, by.length);
            os.write(by, 0, by.length);
            System.out.println(new String(by));
            os.flush();
            dos = new DataOutputStream(os);
            dos.writeUTF(toSend);
            System.out.println(toSend);
            //}
        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }
	//}
     }
}
