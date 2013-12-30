// オークションの管理
import java.util.*;

public class Auction {
	private int goods_num;
	private int[] goods_prices;
	private int[] goods_prices_prev;
	private int client_num;
	private boolean[][] client_bid;
	private boolean[][] client_bid_prev;
	private int priceUnit = 1;
	private int[] winner;

	public Auction(int goods_num, int client_num) {
		this.goods_num = goods_num;
		this.goods_prices = new int[goods_num];
		this.goods_prices_prev = new int[goods_num];
		this.winner = new int[goods_num];

		for (int i=0; i<goods_num; i++) {
			goods_prices[i] = 0;
			winner[i] = -1;
		}

		this.client_num = client_num;
		this.client_bid = new boolean[client_num][goods_num];
		this.client_bid_prev = new boolean[client_num][goods_num];

		for (int i=0; i<client_num; i++) {
			for (int j=0; j<goods_num; j++) {
				client_bid[i][j] = true;
			}
		}
	}

	public int getGoodsNum() {
		return goods_num;
	}

	public int[] getGoodsPrices() {
		return goods_prices;
	}

	public int getClientNum() {
		return client_num;
	}

	public boolean[][] getBids() {
		return client_bid;
	}

	public String getPriceString() {
		String ret = "";

		for (int i=0; i<goods_num; i++) {
			ret += "g" + (i+1) + ":" + goods_prices[i] + " ";
		}
		return ret;
	}

	public String getRoundString() {
		String ret = "";

		for (int i=0; i<goods_num; i++) {
			ret += "g" + (i+1) + ":" + goods_prices_prev[i] + " ";
		}

		for (int i=0; i<client_num; i++) {
			ret += "a" + (i+1) + ":";
			for (int j=0; j<goods_num; j++) {
				if (client_bid[i][j]) {
					ret += "1";
				} else {
					ret += "0";
				}
			}
			if (i < client_num-1) {
				ret += " ";
			}
		}
		return ret;
	}

	public String getWinnerString() {
		String msg = "winner  ";

		for (int i=0; i<goods_num; i++) {
			msg += "g" + (i+1) + ":" + (winner[i]+1) + " ";
		}
		return msg;
	}

	public String getPriceResultString() {
		String msg = "price   ";
		for (int i=0; i<goods_num; i++) {
			msg += "g" + (i+1) + ":" + goods_prices[i] + " ";
		}
		return msg;
	}

	public synchronized void updateBid(String bidString, ClientCommunicator cc) {
		boolean bid[] = new boolean[goods_num];

		if (bidString.length() < goods_num) {
			for (int i=0; i<goods_num; i++) {
				bid[i] = false;
			}
		} else {
			for (int i=0; i<goods_num; i++) {
				if (bidString.charAt(i) == '1') {
					bid[i] = true;
				} else {
					bid[i] = false;
				}
			}
		}
		updateBidBool(bid, cc.getID());
	}

	private void updateBidBool(boolean[] bid, int id) {
		for (int i=0; i<goods_num; i++) {
			client_bid_prev[id][i] = client_bid[id][i];
			client_bid[id][i] = client_bid[id][i] && bid[i];
		}
	}

	// 勝者と価格の更新。オークションが終わったらtrueを返す
	public boolean updateRound() {
		boolean ret = true;

		updateWinner();
		updatePrice();
		for (int i=0; i<goods_num; i++) {
			if (winner[i] < 0) {
				ret = false;
			}
		}
		return ret;
	}

	private void updateWinner() {
		for (int i=0; i<goods_num; i++) {
			if (winner[i] >= 0) {
				continue;
			}

			int bidders = 0;
			for (int j=0; j<client_num; j++) {
				if (client_bid[j][i] && (winner[i]<0)) {
					bidders++;
					winner[i] = j;
				} else if (client_bid[j][i] && (winner[i]>=0)) {
					bidders++;
					winner[i] = -1;
					break;
				}
			}

			// 勝者が決まらないまま全員降りた場合
			if ((bidders==0) && (winner[i]<0)) {
				List winners = new ArrayList();
				for (int j=0; j<client_num; j++) {
					if (client_bid_prev[j][i]) {
						winners.add(new Integer(j));
					}
				}
				winner[i] = ((Integer)winners.get((int)(Math.random() * winners.size()))).intValue();
			}
		}
	}

	private void updatePrice() {
		for (int i=0; i<goods_num; i++) {
			goods_prices_prev[i] = goods_prices[i];
			if (winner[i] < 0) {
				goods_prices[i] += priceUnit;
			}
		}
	}
}
