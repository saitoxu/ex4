public class ServerLauncher {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("usage: Server [port] [client_num] [goods_num]");
            return;
        }

        Server server = new Server(Integer.parseInt(args[0]),
                                   Integer.parseInt(args[1]),
                                   Integer.parseInt(args[2]));
        server.registerClient();
        System.out.println("Start Auction");
        server.startAuction();
    }
}
