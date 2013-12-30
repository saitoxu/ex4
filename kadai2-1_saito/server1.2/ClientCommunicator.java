import java.net.*;
import java.io.*;

public class ClientCommunicator {
    private Socket socket = null;
    private PrintWriter out = null;
    private BufferedReader in = null;
    private int id = 0;
    private String name;

    public ClientCommunicator(Socket socket, int id) {
        this.socket = socket;
        this.id = id;
        try {
            this.out = new PrintWriter(socket.getOutputStream(), true);
            this.in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (Exception e) {
            e.printStackTrace();
        }
        name = null;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public int getID() {
        return id;
    }

    public void sendMessage(String msg) {
        System.out.println("Sending... \"" + msg + "\" to " + name);
        out.println(msg);
        try {
            Thread.sleep(100);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public String readLine() {
        try {
            return in.readLine();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public void close() {
        try {
            in.close();
            out.close();
            socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
