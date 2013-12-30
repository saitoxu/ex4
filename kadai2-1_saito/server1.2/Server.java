import java.net.*;
import java.util.*;

class Server {
    private List clients;
    private int port;
    private int client_num;
    private Auction auction;
    private boolean endAuction;
    private int bidded;
    private int goods_num=5;
    private int registered;

    // ポート番号，クライアント数，商品の数を指定して作成
    public Server(int port, int client_num, int goods_num) {
        this.port = port;
        this.client_num = client_num;
        this.goods_num = goods_num;
        clients = new LinkedList();
    }

    // ポート番号，クライアント数を指定して作成
    public Server(int port, int client_num) {
        this.port = port;
        this.client_num = client_num;
        clients = new LinkedList();
    }

    // ポート番号を指定して生成
    public Server(int port) {
        this.port = port;
        this.client_num = 0;
        clients = new LinkedList();
    }

    // クライアントの登録
    public void registerClient() {
        try {
            ServerSocket serverS = new ServerSocket(port);
            Socket socket;

            System.out.println("ready");
            for (int i=0; i<client_num; i++) {
                socket = serverS.accept();
                addClient(socket, i);
                System.out.println("client registered: " + i);
            }
            registered = 0;
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    // 名前の登録
    public synchronized void registerName(ClientCommunicator cc, String name) {
        registered++;
        cc.setName(name);
        System.out.println(name);
        if (registered >= clients.size()) {
            String nameMsg = "";
            Iterator it = clients.iterator();
            while (it.hasNext()) {
                ClientCommunicator cc2 = (ClientCommunicator)it.next();
                nameMsg += "a" + (cc2.getID()+1) + ":" + cc2.getName() + " ";
            }
            System.out.println(nameMsg);
            this.notifyAll();
        } else {
            try {
                wait();
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    // クライアントの追加
    public void addClient(Socket socket, int id) {
        ClientCommunicator cc = new ClientCommunicator(socket, id);
        clients.add(cc);
    }

    // auctionのゲッター
    public Auction getAuction() {
        return auction;
    }

    // オークションの終了判定
    public boolean isEndAuction() {
        return endAuction;
    }

    // オークションの開始
    public void startAuction() {
        auction = new Auction(goods_num, clients.size());

        bidded = 0;

        Iterator it = clients.iterator();
        while (it.hasNext()) {
            ClientListener cl = new ClientListener(this, (ClientCommunicator)it.next());
            cl.start();
        }
    }

    // 入札完了通知
    public synchronized void endBid() {
        bidded++;

        if (bidded >= clients.size()) {
            // 全員の入札が完了すると，そのラウンドの終了処理を行い，次のラウンドへ

            endAuction = auction.updateRound();
            try {
                Thread.sleep(100);
            }
            catch(Exception e) {
                e.printStackTrace();
            }
            System.out.println("round end" + bidded);
            bidded = 0;
            this.notifyAll();
        } else {
            // 全員の入札が完了するまで待機する．

            try {
                wait();
            }
            catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public List getClients() {
        return clients;
    }
}
