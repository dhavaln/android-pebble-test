package com.jumpbyte.pebbletest.pebble;

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import java.util.UUID;

public class PebbleSenderService extends IntentService {

    public static final String LOGTAG = "PEBBLETEST";
    public static Queue<PebbleDictionary> queue = new LinkedList<PebbleDictionary>();
    public static int index = 0;

    private static AckReceiver ackReceiver = new AckReceiver(PmlBroadcastReceiver.PEBBLE_APP_UUID);
    private static NAckReceiver nAckReceiver = new NAckReceiver(PmlBroadcastReceiver.PEBBLE_APP_UUID);

    public static class AckReceiver extends PebbleKit.PebbleAckReceiver{

        public AckReceiver(UUID uuid){
            super(uuid);
        }

        @Override
        public void receiveAck(Context context, int transactionId) {
            Log.i(LOGTAG, "Received ack for transaction " + transactionId);

            try { Thread.sleep(20); } catch (Exception ex) {}
            PebbleDictionary msg = queue.poll();
            if (msg != null) {
                PebbleKit.sendDataToPebbleWithTransactionId(context, PmlBroadcastReceiver.PEBBLE_APP_UUID, msg, index);
                index++;
            }
        }
    }

    public static class NAckReceiver extends PebbleKit.PebbleNackReceiver{

        public NAckReceiver(UUID uuid){
            super(uuid);
        }

        @Override
        public void receiveNack(Context context, int transactionId) {
            Log.i(LOGTAG, "Received nack for transaction " + transactionId);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();

        PebbleKit.registerReceivedAckHandler(getApplicationContext(), ackReceiver);
        PebbleKit.registerReceivedNackHandler(getApplicationContext(), nAckReceiver);
    }

    @Override
    protected void onHandleIntent(Intent intent) {
        ArrayList<PebbleDictionary> messages = (ArrayList<PebbleDictionary>) intent.getSerializableExtra("messages");
        for (PebbleDictionary message : messages) queue.offer(message);
        PebbleKit.sendDataToPebble(this, PmlBroadcastReceiver.PEBBLE_APP_UUID, queue.poll());
    }

    public PebbleSenderService() {
        super("Pebble Sender Service");
    }
}
