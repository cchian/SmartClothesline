package com.cchian.clotheslineagent;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;

public class SettingActivity extends AppCompatActivity {

    SharedPreferences sp;// = getSharedPreferences(name, mode);
    SharedPreferences.Editor editor;// = sp.edit();

    private String appid ; //APP_ID
    private String key;//KEY
    private String secret ; //SECRET
    private String recv;
    private int notifyDuration;//600s (10mins)


    private EditText txtAppID,txtKey,txtSecret,txtRecv,txtNotiifyDuration;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_setting);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

//        FloatingActionButton fab = findViewById(R.id.fab);
//        fab.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View view) {
//                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
//                        .setAction("Action", null).show();
//            }
//        });


        // add back arrow to toolbar
        if (getSupportActionBar() != null){
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setDisplayShowHomeEnabled(true);
        }


        sp=getSharedPreferences("ctrv_config", Context.MODE_PRIVATE);
        editor=sp.edit();
        appid = sp.getString("id", "");
        key=sp.getString("key","");
        secret=sp.getString("secret","");
        recv=sp.getString("recv","");
        notifyDuration=sp.getInt("notify",1);

        txtAppID=(EditText)findViewById(R.id.txtAppID);
        txtKey=(EditText)findViewById(R.id.txtKey);
        txtSecret=(EditText)findViewById(R.id.txtSecret);
        txtRecv=(EditText)findViewById(R.id.txtRecv);
        txtNotiifyDuration=(EditText)findViewById(R.id.txtNotifyRate);

        txtAppID.setText(appid);
        txtKey.setText(key);
        txtSecret.setText(secret);
        txtRecv.setText(recv);
        txtNotiifyDuration.setText(notifyDuration+"");
    }

    public void onResume() {
        super.onResume();


    }



    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // handle arrow click here
        if (item.getItemId() == android.R.id.home) {
            saveSetting();
            finish(); // close this activity and return to preview activity (if there is any)
        }

        return super.onOptionsItemSelected(item);
    }

    private void saveSetting(){
        editor.putString("id", txtAppID.getText()+"");
        editor.putString("key",txtKey.getText()+"");
        editor.putString("secret",txtSecret.getText()+"");
        editor.putString("recv",txtRecv.getText()+"");
        editor.putInt("notify",Integer.parseInt(txtNotiifyDuration.getText()+""));

        editor.commit();
    }

}
