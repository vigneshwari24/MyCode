import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
public class MDNS {
    public static void main(String[] args)  {
        Socket s;
        try {
            ServerSocket ss = new ServerSocket(5070);
            //System.out.println("SS created");
            while(true) {
              s = ss.accept();
              Dns dns = new Dns(s);
           // System.out.println("listening");
            }
        } catch(Exception e) {
            System.out.println(e.getMessage());
        }
    }
}

class Dns extends Thread {
    Socket s1;
    public Dns(Socket s) {
       s1 = s;
       this.start();
    }
    @Override
    public void run() {
        try {
            InputStream is = s1.getInputStream();
            DataInputStream dis = new DataInputStream(is);
            String recvd = dis.readUTF();
            System.out.println(recvd);
            String arr[] = new String[3];
            arr = recvd.split(" ");
            Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
            Connection con = DriverManager.getConnection("jdbc:odbc:dns");
           	Statement st = con.createStatement();
         	ResultSet rs = st.executeQuery("select * from dns where url = '" +arr[0]+ "'");
            rs.next();
            String sip = rs.getString("ip");
            int sport = Integer.parseInt(rs.getString("port"));
            Socket s11  = new Socket(sip,sport);
            OutputStream os = s11.getOutputStream();
            DataOutputStream dos = new DataOutputStream(os);
            dos.writeUTF(recvd);
            con.close();
        } catch(Exception e) {
            System.out.println(e.getMessage());
        }
    }

}
