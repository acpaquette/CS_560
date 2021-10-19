import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class EchoThread implements Runnable {

    private Socket socket;
    public EchoThread(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        handleClient(socket);
    }

    public void handleClient(Socket clientSocket) {

        DataInputStream fromClient = null;
        DataOutputStream toClient = null;

        int charFromClient = 0;
        int state = 0;
        boolean keepGoing = true;

        // show that we are connected to client
        System.out.println("Client connected ...");

        // first get the streams
        try {
            fromClient = new DataInputStream(clientSocket.getInputStream());
            toClient = new DataOutputStream(clientSocket.getOutputStream());
        } catch (IOException e) {
            System.err.println("Error opening network streams");
            return;
        }

        // now talk to the client
        while (keepGoing) {
            try {
                charFromClient = fromClient.readByte();
                System.out.print((char)charFromClient);
            } catch (IOException e) {
                System.err.println("Error reading character from client");
                return;
            }

            try {
                toClient.writeByte(charFromClient);
            } catch (IOException e) {
                System.err.println("Error writing character to client");
                return;            }

            if (charFromClient == 'q') {
                System.out.println("\nBailing out!");
                keepGoing = false;
            }
        }

        try {
            clientSocket.close();
        } catch (IOException e) {
            System.err.println("Error closing socket to client");
        }

    }

}
