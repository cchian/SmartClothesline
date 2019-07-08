package com.cchian.clotheslineagent;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.media.AudioAttributes;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Bundle;
import android.os.Messenger;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import androidx.core.app.NotificationCompat;
import io.netpie.microgear.*;
import static android.media.AudioAttributes.CONTENT_TYPE_SONIFICATION;

public class MainActivity extends Activity implements View.OnClickListener, Runnable{

    private byte CMD_MANU=0;
    private byte CMD_AUTO=1;
    private byte CMD_ON=2;
    private byte CMD_OFF=3;
    private byte CMD_RAW=4;

    private TextView txtRaindriop,txtLuxmeter;
    private Button btnOn,btnOff;
    private Switch swAuto;
    private ImageView imgIndecator,imgSetting;

    private Microgear microgear;
    private String appid = "proj4u"; //APP_ID
    private String key = "h1sAAeIkJN06CbM" ;//KEY
    private String secret = "CtEPQT5IamWJizjSkSoQEH5fZ"; //SECRET

    private String alias = "CtrlV";

    private String msgTemplate="";
    private String recv="CtrC";

    private Thread threadStatusRunning;
    private  boolean threadStatusRunningInterrupt=false;

    private boolean active=false;

    private boolean notifyRain =false;
    private boolean getNotifyLux=false;

    SharedPreferences sp;// = getSharedPreferences(name, mode);
    SharedPreferences.Editor editor;// = sp.edit();

    private int notifyDuration=5*60;//600s (10mins)


    private void recoverySetting(){
        sp=getSharedPreferences("ctrv_config",Context.MODE_PRIVATE);
        appid=sp.getString("id","");
        key=sp.getString("key","");
        secret=sp.getString("secret","");
        recv=sp.getString("recv","");
        notifyDuration=sp.getInt("notify",1)*60;
    }
    private void initMicrogear(){
        recoverySetting();;

        msgTemplate = alias + "#" + recv + "#";

        if(!appid.equals("")&&!key.equals("")&&!secret.equals("")&&!recv.equals("")){
            microgear.connect(appid, key, secret, alias);
        }
        microgear.subscribe("chat");
        sendMessage(msgTemplate + (CMD_RAW));
    }
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initWidget();


         microgear= new Microgear(this);
        MicrogearCallBack callback = new MicrogearCallBack();
        microgear.setCallback(callback);
        initMicrogear();

        threadStatusRunning = new Thread(this);
        threadStatusRunning.start();
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Bundle bundle = msg.getData();
            String message = bundle.getString("myKey");
            TextView myTextView = (TextView) findViewById(R.id.txtConsole);
            myTextView.setText(message + "\n");
            // $msgCtrlC#CtrlV#1,1,rrrr,lllll
            if (message.startsWith("$msg")) {
                message=message.replace("$msg","");
                if(message.split("#")[1].equals(alias)){
                    String msgArrays[]=message.split("#")[2].split(",");
                    if(msgArrays[0].equals("0")){
                        Log.i("state","false");
                        swAuto.setChecked(false);
                    }else{
                        Log.i("state","true");
                        swAuto.setChecked(true);
                    }
                    //แดงอ่อนๆ 0xFFCFBBBB
                    //แดง 0xFFFF2222

                    //สีเขียวอ่อนๆ  0xFF87AD88
                    //สีเขียว  0xFF07AF0E

                    if(msgArrays[1].equals("1")){
                        runOnUiThread(new Runnable() {// start actions in UI thread
                            @Override
                            public void run() {
                                imgIndecator.setImageResource(R.drawable.dark_green);
                                btnOn.setBackgroundColor(0xFF87AD88);
                                btnOff.setBackgroundColor(0xFFFF2222);
                                active=true;
                            }
                        });
                    }else{
                        runOnUiThread(new Runnable() {// start actions in UI thread
                            @Override
                            public void run() {
                                imgIndecator.setImageResource(R.drawable.blank);
                                   btnOn.setBackgroundColor(0xFF07AF0E);
                                btnOff.setBackgroundColor(0xFFCFBBBB);
                                active=false;
                            }
                        });
                    }

                    txtRaindriop.setText(msgArrays[2]);
                    txtLuxmeter.setText(msgArrays[3]);
                }
            }else if(message.startsWith("$notify")){
                message=message.replace("$notify","");
                Log.i("message",">>>>>>>>>>>>>>>>>>>>>>"+message);
                if(message.split("#")[1].equals(alias)){

                    String strNotify=message.split("#")[2];
                    if(strNotify.equals("rainy")){
                        if(!notifyRain){
                            notifyRain=true;showNotification("ตรวจพบฝน","..........");
                            new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    for(int i=0;i<notifyDuration;i++){
                                        try {
                                            Thread.sleep(1000);
                                        } catch (InterruptedException e) {

                                        }
                                    }
                                    notifyRain=false;
                                }
                            }).start();
                        }
                    }else if(strNotify.equals("cloudy")){
                        if(!getNotifyLux){
                            getNotifyLux=true;
                            notifyRain=true;
                            showNotification("สภาพท้องฟ้า","..........");
                            new Thread(new Runnable() {
                                @Override
                                public void run() {
                                    for(int i=0;i<notifyDuration;i++){
                                        try {
                                            Thread.sleep(1000);
                                        } catch (InterruptedException e) {

                                        }
                                    }
                                    getNotifyLux=false;
                                }
                            }).start();
                        }
                    }
                }
            }
        }
    };


    class MicrogearCallBack implements MicrogearEventListener{

        @Override
        public void onConnect() {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", "เชื่อมต่อกับ netpie แล้ว");
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("Connected","ไม่ได้เชื่อมต่อ netpie");
        }

        @Override
        public void onMessage(String topic, String message) {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", message);
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("Message",message);
        }

        @Override
        public void onPresent(String token) {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", "New friend Connect :"+token);
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("present","New friend Connect :"+token);
        }

        @Override
        public void onAbsent(String token) {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", "Friend lost :"+token);
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("absent","Friend lost :"+token);
        }

        @Override
        public void onDisconnect() {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", "Disconnected");
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("disconnect","Disconnected");
        }

        @Override
        public void onError(String error) {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", "Exception : "+error);
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("exception","Exception : "+error);
        }

        @Override
        public void onInfo(String info) {
            Message msg = handler.obtainMessage();
            Bundle bundle = new Bundle();
            bundle.putString("myKey", "Exception : "+info);
            msg.setData(bundle);
            handler.sendMessage(msg);
            Log.i("info","Info : "+info);
        }
    }

    public void onClick(View v) {
        if (v == swAuto) {
            if (swAuto.isChecked()) {
                Log.i("test", "Auto");
                sendMessage(msgTemplate + (CMD_AUTO));
            } else {
                Log.i("test", "Manual");
                sendMessage(msgTemplate + (CMD_MANU));
            }
        }else if (v == btnOn || v == btnOff) {
            if (v == btnOn) {
                if (!swAuto.isChecked()) {
                    if(active!=true) {
                        Log.i("test", "ON");
                        sendMessage(msgTemplate + (CMD_ON));
                    }else{
                        Toast.makeText(MainActivity.this, "ราวตากผ้าเปิดอยู่แล้ว", Toast.LENGTH_LONG).show();
                    }
                } else {
                    Log.i("test", "Please Turnoff Auto Running..");
                    Toast.makeText(MainActivity.this, "ปิดระบบอัจฉริยะก่อน", Toast.LENGTH_LONG).show();
                }
            }else if (v == btnOff) {
                if (!swAuto.isChecked() ) {
                    if(active!=false) {
                        Log.i("test", "OFF");
                        sendMessage(msgTemplate + (CMD_OFF));
                    }else{
                        Toast.makeText(MainActivity.this, "ราวตากผ้าปิดอยู่แล้ว", Toast.LENGTH_LONG).show();
                    }

                } else {
                    Log.i("test", "Please Turnoff Auto Running..");
                    Toast.makeText(MainActivity.this, "ปิดระบบอัจฉริยะก่อน", Toast.LENGTH_LONG).show();
                }
            }
        }else if(v==imgSetting){
            //Toast.makeText(MainActivity.this, "การตั้งค่า", Toast.LENGTH_LONG).show();
            Log.i("test", "Go Setting.......");
            Intent intent=new Intent(this,SettingActivity.class);
            startActivity(intent);
        }
    }

    private void initWidget(){
        swAuto=(Switch)findViewById(R.id.swAuto);
        btnOn=(Button)findViewById(R.id.btnOn);
        btnOff=(Button)findViewById(R.id.btnOff);

        swAuto.setOnClickListener(this);
        btnOn.setOnClickListener(this);
        btnOff.setOnClickListener(this);


        txtRaindriop=(TextView)findViewById(R.id.txtRaindrop);
        txtLuxmeter=(TextView)findViewById(R.id.txtLuxmeter);
        imgIndecator=(ImageView)findViewById(R.id.imageIndecator);

        imgSetting=(ImageView)findViewById(R.id.imgSetting);
        imgSetting.setOnClickListener(this);


        btnOn.setBackgroundColor(0xFF07AF0E);
        btnOff.setBackgroundColor(0xFFCFBBBB);
    }

    private String messageBuffer="";

    private void sendMessage(String msg){
        messageBuffer=msg;
        try{
            new Thread(new Runnable(){ // start actions in UI thread
                @Override
                public void run(){
                    microgear.publish("chat", messageBuffer);
                }
            }).start();
            // Thread.sleep(2000);
        }
        catch (Exception e){
            // ooops
        }
    }

    @Override
    public void run() {
        while (!threadStatusRunningInterrupt) {
           try {
               sendMessage(msgTemplate + (CMD_RAW));
               if(swAuto.isChecked()) {
                       runOnUiThread(new Runnable() {// start actions in UI thread
                           @Override
                           public void run() {
                               imgIndecator.setImageResource(R.drawable.dark_green);
                           }
                       });
                       Thread.sleep(200);
                       runOnUiThread(new Runnable() {// start actions in UI thread
                           @Override
                           public void run() {
                               imgIndecator.setImageResource(R.drawable.blank);
                           }
                       });
                       System.out.println("thread running...");
               }
               Thread.sleep(2000);
           } catch (InterruptedException e) {
            }
        }
    }

    void showNotification(String title, String message) {
        Uri soundUri = Uri.parse(ContentResolver.SCHEME_ANDROID_RESOURCE + "://"+ getApplicationContext().getPackageName() + "/" + R.raw.thunder);

        NotificationManager mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel("YOUR_CHANNEL_ID",
                    "YOUR_CHANNEL_NAME",
                    NotificationManager.IMPORTANCE_DEFAULT);
            channel.setDescription("YOUR_NOTIFICATION_CHANNEL_DISCRIPTION");
            mNotificationManager.createNotificationChannel(channel);
        }
        @SuppressLint("WrongConstant") NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(getApplicationContext(), "YOUR_CHANNEL_ID")
                .setSound(soundUri, CONTENT_TYPE_SONIFICATION)
                .setSmallIcon(R.mipmap.ic_launcher) // notification icon
                .setContentTitle(title) // title for notification
                .setContentText(message)// message for notification
                .setAutoCancel(true); // clear notification after click
        Intent intent = new Intent(getApplicationContext(), MainActivity.class);
        PendingIntent pi = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);
        mBuilder.setContentIntent(pi);
        mNotificationManager.notify(0, mBuilder.build());
    }

    protected void onDestroy() {
        super.onDestroy();
        microgear.disconnect();
    }

    protected  void onPause(){
        super.onPause();

        if(threadStatusRunning!=null) {
            threadStatusRunningInterrupt=true;
            threadStatusRunning=null;
            System.out.println("pause status running thread.......");
        }

    }

    protected void onResume() {
        super.onResume();
        recoverySetting();
        msgTemplate = alias + "#" + recv + "#";
        microgear.bindServiceResume();
        threadStatusRunningInterrupt=false;
       if(threadStatusRunning==null){
           threadStatusRunning=new Thread(this);
           threadStatusRunning.start();
       }
    }


}