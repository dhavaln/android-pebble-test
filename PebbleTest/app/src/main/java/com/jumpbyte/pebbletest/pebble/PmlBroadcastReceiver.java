package com.jumpbyte.pebbletest.pebble;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;
import com.google.common.primitives.Ints;

import java.math.BigInteger;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.UUID;

public class PmlBroadcastReceiver extends PebbleKit.PebbleDataReceiver {

    private static final String LOGTAG = "PEBBLETEST";

    public static final UUID PEBBLE_APP_UUID = UUID.fromString("4ccae58f-779c-4270-b82a-cb5aa6be4cc1");
    // Keys
    private static final int MSG_TYPE = 0;
    private static final int MSG_CTX = 1;
    private static final int MSG_PARENT_CTX = 2;
    private static final int COUNT = 100;
    private static final int INDEX = 101;
    private static final int ID = 102;
    private static final int NAME = 103;
    private static final int SERVICEID = 104;
    private static final int LINK = 105;

    private static final short REQ = 0;
    private static final short RSP_START = 1;
    private static final short RSP_DATA = 2;

    private static final short DATALIST = 0;

    public static final int ADD_EXTRA = 20;

    public PmlBroadcastReceiver() {
        this.subscribedUuid = PEBBLE_APP_UUID;
    }

    private int getInteger(final PebbleDictionary data, int key) {
        try {
            return Ints.checkedCast(data.getUnsignedInteger(key));
        } catch (RuntimeException ex) {
            try {
                return Ints.checkedCast(data.getInteger(key));
            } catch (RuntimeException e) {
                return -1;
            }
        }
    }

    @Override
    public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
        PebbleKit.sendAckToPebble(context, transactionId);

        int msgType = getInteger(data, MSG_TYPE);
        int msgCtx = getInteger(data, MSG_CTX);
        int msgParentCtx = getInteger(data, MSG_PARENT_CTX);

        Log.i(LOGTAG, "Got Pebble Msg " + msgType + ", " + msgCtx + ", " + msgParentCtx);

        if (msgType == REQ) {
            sendList(context, msgParentCtx);
        }
    }

    private PebbleDictionary countMessage(final short context, final int size) {
        PebbleDictionary countMsg = new PebbleDictionary();
        countMsg.addUint16(MSG_TYPE, RSP_START);
        countMsg.addUint16(MSG_CTX, context);
        countMsg.addUint16(COUNT, (short) size);
        return countMsg;
    }

    private PebbleDictionary entryMessage(final short context, final short index, final long id, final String name, final String serviceId, final String link) {
        PebbleDictionary entryMsg = new PebbleDictionary();
        entryMsg.addUint16(MSG_TYPE, RSP_DATA);
        entryMsg.addUint16(MSG_CTX, context);
        entryMsg.addUint16(INDEX, index);
        entryMsg.addUint32(ID, Ints.checkedCast(id));
        entryMsg.addString(NAME, name);
        entryMsg.addString(SERVICEID, serviceId);
        entryMsg.addString(LINK, link);
        Log.i(LOGTAG, "message size " + entryMsg.toJsonString().length());
        return entryMsg;
    }

    private void startSending(final Context context, final ArrayList<PebbleDictionary> messages) {
        Intent intent = new Intent(context, PebbleSenderService.class);
        intent.putExtra("messages", messages);
        intent.putExtra("uuid", PEBBLE_APP_UUID);
        context.startService(intent);
    }

    private SecureRandom random = new SecureRandom();

    public String nextSessionId() {
        return new BigInteger(100, random).toString(32);
    }

    private void sendList(final Context context, int count) {
        ArrayList<PebbleDictionary> messages = new ArrayList<PebbleDictionary>();

        short index = 0;
        messages.add(countMessage(DATALIST, count + ADD_EXTRA));
        for (int i=0;i<(count + ADD_EXTRA);i++) {
            String name = i + " " + nextSessionId();
            messages.add(entryMessage(DATALIST, index, 0, name, "Data List", "testkey:testid:1zCNrbPpz5OLSr6mSpPdKm"));
            index += 1;
        }

        startSending(context, messages);
    }
}
