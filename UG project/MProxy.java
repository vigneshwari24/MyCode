import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Date;

public class MProxy {
    public static void main(String args[]) throws Exception
    {
    ServerSocket ss;
    Socket s;
    ss = new ServerSocket(5050);
            while(true)
            {
                s = ss.accept();
                vikki v=new vikki(s);
            }
    }		
	
}
class vikki extends Thread
{
    InputStream is;
	OutputStream os;
    public static OutputStream osrc;
	DataInputStream dis,dis1,dis2;
	DataOutputStream dos;
	InetAddress ia;
	Connection con;
	Statement st;
	ResultSet rs;
    Socket s,s2,s5,s6,ns1;
	String ttime,toSend;
	public static String url;
	Date d;
    Socket ns;
    public vikki(Socket s1)
    {
          s=s1;
          this.start();
    }
    @Override
    public void run()
    {
     try{
                 ia = InetAddress.getLocalHost();
			toSend = ia.getHostAddress().concat(" ")/*.concat("5040")*/;
			d = new Date();
		  	Integer hr = new Integer(d.getHours());
		   	Integer min = new Integer(d.getMinutes());
	   		ttime = hr.toString().concat(":").concat(min.toString());
			Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
           	con = DriverManager.getConnection("jdbc:odbc:proxy");
           	st = con.createStatement();
	
	//void connectToClient() throws Exception {

        is = s.getInputStream();
        dis = new DataInputStream(is);
    	url = dis.readUTF();
		osrc = s.getOutputStream();
		toSend = url.concat(" ").concat(toSend);
	//}

	//void serveRequest () throws Exception {
   		rs = st.executeQuery("select * from proxy where url = '" +url+ "'");
		if(rs.next()) {
			if(rs.getString("flag").equals("true")) {
				//sendStaticFile();
                FileInputStream fis = new FileInputStream(url);
                File myFile = new File(url);
                BufferedInputStream bis = new BufferedInputStream(fis);
                byte by[] = new byte[(int)myFile.length()];
            bis.read(by,0,by.length);
		osrc.write(by,0,by.length);
        int i = st.executeUpdate("update proxy set count = count + 1,proxy.time = '" +ttime+ "'  where url = '" +url+"'");
        con.close();
			}
			else {
				//sendDynamicFile();
                 String port = rs.getString("port");
        String ip = rs.getString("ip");
		s2 = new Socket(ip,Integer.parseInt(port));
        System.out.println(port);
        System.out.println(ip);
		os = s2.getOutputStream();

		dos = new DataOutputStream(os);
        toSend = toSend.concat("5090");
		dos.writeUTF(toSend);
        System.out.println(toSend);
	//	byte by1[] = new byte[100];
        ServerSocket ss1 = new ServerSocket(5090);
        while(true) {
		 ns = ss1.accept();
         v obj = new v(ns);
         System.out.println("Before update");
         Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
           	con = DriverManager.getConnection("jdbc:odbc:proxy");
           	st = con.createStatement();
        int i = st.executeUpdate("update proxy set count = count + 1,proxy.time = '" +ttime+ "'  where url = '" +url+"'");
        System.out.println("after update");
         con.close();
         
        }
  			}
			
		}
		else {
			//connectToDNS();
           s5 = new Socket("10.6.2.55",5070);
            //s5 = new Socket("localhost",5070);
            os = s5.getOutputStream();
            dos = new DataOutputStream(os);
            String temp = ia.getHostAddress().concat(" ").concat("5100");
            String durl = url.concat(" ").concat(temp);
            dos.writeUTF(durl);
            System.out.println(durl);
	        con.close();
            ServerSocket ss1 = new ServerSocket(5100);
            while(true)
            {
                ns1 = ss1.accept();
                Serv serv = new Serv(ns1);
                System.out.println("Inside connecttoDns loop");
            }
        }
     }catch(Exception e){System.out.println(e.getMessage());}

       
    }


}


class v extends Thread {
     Socket s1;
    private byte[] by1 = new byte[100];
    private DataInputStream dis1;
    //String url;
    public v(Socket s) {
            s1=s;
           // this.url = u;
          this.start();

    }
    @Override
    public void run() {
        try {
             InputStream i = s1.getInputStream();
        //InputStream i = s.getInputStream();
		int bytes = i.read(by1,0,by1.length);
		dis1 = new DataInputStream(i);
		String serinfo = dis1.readUTF();

		vikki.osrc.write(by1,0,bytes);
        System.out.println(new String(by1));


        }catch(Exception e) {
            System.out.println(e.getMessage());
        }
    }
}
class Serv extends Thread {
    Socket s1;
    String url,ttime;

    public Serv(Socket s) {
        s1 = s;
       // ttime = time;
        //this.url = url;
        this.start();
    }
    @Override
    public void run() {
        try {
        byte by2[] = new byte[100];
        Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
        Connection con = DriverManager.getConnection("jdbc:odbc:proxy");
        Statement st = con.createStatement();
		InputStream is2 = s1.getInputStream();
		int bytes = is2.read(by2,0,by2.length);
        System.out.println(new String(by2));
        vikki.osrc.write(by2,0,bytes);
        DataInputStream dis2 = new DataInputStream(is2);
		String serinfo1 = dis2.readUTF();
		String ser[] = new String[2];
		ser = serinfo1.split(" ");
		int serport = Integer.parseInt(ser[1]);
        st.close();
        con.commit();
        con.close();

		lru(url,ser[0],serport,ttime);
        } catch(Exception e) {
            System.out.println(e.getMessage());
        }

    }
    public void lru(String url,String ip,int serport,String ttime) { try {
        Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
        Connection con1 = DriverManager.getConnection("jdbc:odbc:proxy");
        Statement st1 = con1.createStatement();
        ResultSet rs1 = st1.executeQuery("select count(url) from proxy");
		rs1.next();
		if(Integer.parseInt(rs1.getString(1))  >= 10) {
            System.out.println("LRU");
			rs1 = st1.executeQuery("select url from proxy where time = (select min(time) from proxy)");
			rs1.next();
			int i = st1.executeUpdate("delete from proxy where url = '" +rs1.getString(1)+ "'");
           // int i1 = st1.executeUpdate("insert into proxy values ( '" +url+ "', '" +ip+ "',  " +serport+ " , '" +false+ "', 0, '" +ttime+ "')");
        }
        System.out.println("Insert statement");
        // int i1 = st1.executeUpdate("insert into proxy values ( ' url', 'ip ',  7 , '" +false+ "', 0, '" +ttime+ "')");
          // i1 = st1.executeUpdate("insert into proxy values ( ' url', 'ip ',  7 , '" +false+ "', 0, '" +ttime+ "')");
          System.out.println("Insert statement");
	url = vikki.url;
	System.out.println(url); 
	Date d = new Date();
		  	Integer hr = new Integer(d.getHours());
		   	Integer min = new Integer(d.getMinutes());
	   		ttime = hr.toString().concat(":").concat(min.toString());

	String query = "insert into proxy values ( '" +url+ "', '" +ip+ "',  " +serport+ " , '" +false+ "', 0, '" +ttime+ "')";
       int i1 = st1.executeUpdate(query);
       System.out.println(query);

  

        
		
       // con1.commit();
        con1.close();
        st1.close();
        System.out.println("Inserted");
        this.stop();
    }catch(Exception e) {
        System.out.println(e);
    }
    }
 }
