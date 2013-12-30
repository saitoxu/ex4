import java.net.*;
import java.io.*;

class ServerThread extends Thread {
	Socket clientS;
	int sleepTime;

	public ServerThread(Socket acceptedS, int msec) {
		clientS = acceptedS;
		sleepTime = msec;
	}

	public void run() {
		try {
			PrintStream toClient = new PrintStream(clientS.getOutputStream(), true);
			while (true) {
				try {
					sleep(sleepTime);
				} catch (InterruptedException e) {}
				toClient.println(Math.random());
			}
		} catch (IOException e) {
			System.out.println("IO exception.");
			System.exit(1);
		}
	}
}
