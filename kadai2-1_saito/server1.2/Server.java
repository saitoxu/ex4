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

    // �|�[�g�ԍ��C�N���C�A���g���C���i�̐����w�肵�č쐬
    public Server(int port, int client_num, int goods_num) {
        this.port = port;
        this.client_num = client_num;
        this.goods_num = goods_num;
        clients = new LinkedList();
    }

    // �|�[�g�ԍ��C�N���C�A���g�����w�肵�č쐬
    public Server(int port, int client_num) {
        this.port = port;
        this.client_num = client_num;
        clients = new LinkedList();
    }

    // �|�[�g�ԍ����w�肵�Đ���
    public Server(int port) {
        this.port = port;
        this.client_num = 0;
        clients = new LinkedList();
    }

    // �N���C�A���g�̓o�^
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

    // ���O�̓o�^
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

    // �N���C�A���g�̒ǉ�
    public void addClient(Socket socket, int id) {
        ClientCommunicator cc = new ClientCommunicator(socket, id);
        clients.add(cc);
    }

    // auction�̃Q�b�^�[
    public Auction getAuction() {
        return auction;
    }

    // �I�[�N�V�����̏I������
    public boolean isEndAuction() {
        return endAuction;
    }

    // �I�[�N�V�����̊J�n
    public void startAuction() {
        auction = new Auction(goods_num, clients.size());

        bidded = 0;

        Iterator it = clients.iterator();
        while (it.hasNext()) {
            ClientListener cl = new ClientListener(this, (ClientCommunicator)it.next());
            cl.start();
        }
    }

    // ���D�����ʒm
    public synchronized void endBid() {
        bidded++;

        if (bidded >= clients.size()) {
            // �S���̓��D����������ƁC���̃��E���h�̏I���������s���C���̃��E���h��

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
            // �S���̓��D����������܂őҋ@����D

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
