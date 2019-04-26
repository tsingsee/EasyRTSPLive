/*
	Copyright (c) 2012-2018 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyrtmp_rtsp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.support.v4.content.LocalBroadcastManager;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;

public class StatusInfoView extends View {
    private Paint mPaint;
    private Context mContext;
    private static ArrayList<String> mInfoList = null;
    private static StatusInfoView mInstence;
    private static final String TAG = "StatusInfoView";
    public static final String DBG_MSG = "dbg-msg";
    public static final String DBG_DATA = "dbg-data";

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (DBG_MSG.equals(intent.getAction())) {
                final String msg = intent.getStringExtra(DBG_DATA);
                if(!msg.isEmpty()) {
                    addInfoMsg(msg);
                }
            }
        }
    };

    public StatusInfoView(Context context) {
        super(context);
        mContext = context;
        init();
    }

    public StatusInfoView(Context context, AttributeSet attr) {
        super(context,attr);
        mContext = context;
        init();
    }

    private void init(){
        mInfoList = new ArrayList<String>();
        mInfoList.clear();

        final IntentFilter inf = new IntentFilter(DBG_MSG);
        LocalBroadcastManager.getInstance(mContext).registerReceiver(mReceiver, inf);
    }

    public void uninit(){
        LocalBroadcastManager.getInstance(mContext).unregisterReceiver(mReceiver);
    }

    public static StatusInfoView getInstence(){
        return mInstence;
    }

    public void setInstence(StatusInfoView instence){
        mInstence = instence;
        handler.postDelayed(runnable, 1000);
    }

    public void addInfoMsg(String info){
        mInfoList.add(info);
    }

    Handler handler=new Handler();
    Runnable runnable=new Runnable(){
        @Override
        public void run() {
            invalidate();
            handler.postDelayed(this, 1000);
        }
    };

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int i = 0;
        mPaint = new Paint();
        mPaint.setTextSize(30);

        ViewGroup.LayoutParams lp = this.getLayoutParams();
        int posY = this.getHeight() - 30;
        for(i = mInfoList.size()-1; i >= 0; i--){
            if(posY < 0) {
                mInfoList.remove(i);
                continue;
            }

            String info = mInfoList.get(i);
            mPaint.setColor(Color.BLACK);

            canvas.drawText(info, 30, posY , mPaint);
            posY -= 35;
        }
    }
}
