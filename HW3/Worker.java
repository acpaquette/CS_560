package web;


import java.io.*;
import java.net.*;
import java.util.*;

class Worker extends Thread implements HttpConstants {

    final static int BUF_SIZE = 2048;
    static final byte[] EOL = {(byte) '\r', (byte) '\n'};

    /* buffer to use for requests */
    byte[] buffer;
    /* Socket to client we're handling */
    private Socket socket;
    /* WebServer the worker works for */
    WebServer webServer;

    Worker(WebServer webServer, String name) {
        super(name);
        this.webServer = webServer;
        buffer = new byte[BUF_SIZE];
        this.start();
    }

    synchronized void setSocket(Socket socket) {
        this.socket = socket;
        notify();
    }

    public synchronized void run() {

        webServer.workerHasStarted();

        while (true) {
            /* nothing to do */
            try {
                wait();
            } catch (InterruptedException e) {
                /* should not happen */
                continue;
            }

            try {
                handleClient();
            } catch (Exception e) {
                e.printStackTrace();
            }
            /* go back in wait queue if there's fewer
             * than numHandler connections.
             */

            Vector pool = webServer.threads;
            synchronized (pool) {
                if (pool.size() >= webServer.workers) {
                    /* too many threads, exit this one */
                    return;
                } else {
                    pool.addElement(this);
                }
            }
        }
    }

    void handleClient() throws IOException {
        InputStream is = socket.getInputStream();
        PrintStream ps = new PrintStream(socket.getOutputStream());

        try {
            BufferedReader
                    reader = new BufferedReader(new InputStreamReader(is));
            while (true) {
                String line = reader.readLine();
                if (line == null) {
                    break;
                }

                String numStr = line.strip();

                int receivedInt = Integer.parseInt(numStr);
                int iterations = 0;
                while (receivedInt != 1) {
                    switch (receivedInt % 2) {
                        case 1:
                            receivedInt = (3 * receivedInt) + 1;
                            break;
                        case 0:
                            receivedInt /= 2;
                            break;
                    }
                    iterations += 1;
                }

                // System.out.println(iterations);
                ps.println(iterations);
            }
        } finally {
            socket.close();
        }
    }

}
