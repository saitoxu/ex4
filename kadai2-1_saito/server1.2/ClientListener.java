import java.util.*;

public class ClientListener extends Thread {
    private ClientCommunicator cc;
    private Server server;

    public ClientListener(Server server, ClientCommunicator cc) {
        this.server = server;
        this.cc = cc;
    }

    public void startAuction(Auction auction) {
        String bid;

        cc.sendMessage("Your ID: " + (cc.getID()+1));
        cc.sendMessage(auction.getGoodsNum() + "," + auction.getClientNum());
        bid = cc.readLine();
        System.out.println(bid);
        auction.updateBid(bid, cc);
        server.endBid();
    }

    private void regName() {
        String name;

        cc.sendMessage("PLEASE INPUT YOUR NAME");
        name = cc.readLine();
        server.registerName(cc, name);
        String nameMsg = "";
        Iterator it = server.getClients().iterator();
        while (it.hasNext()) {
            ClientCommunicator cc = (ClientCommunicator)it.next();
            nameMsg += "a" + (cc.getID()+1) + ":" + cc.getName() + " ";
        }
        cc.sendMessage(nameMsg);
    }

    private void nextRound(Auction auction) {
        String bid;

        cc.sendMessage(auction.getRoundString());
        cc.sendMessage(auction.getPriceString());
        bid = cc.readLine();
        auction.updateBid(bid, cc);
        server.endBid();
    }

    private void endAuction(Auction auction) {
        cc.sendMessage(auction.getRoundString());
        cc.sendMessage("end");
        cc.sendMessage(auction.getWinnerString());
        cc.sendMessage(auction.getPriceResultString());
    }

    public void run() {
        regName();
        startAuction(server.getAuction());

        while(!server.isEndAuction()) {
            nextRound(server.getAuction());
        }
        endAuction(server.getAuction());
        cc.close();
    }
}
