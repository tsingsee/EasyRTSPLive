package org.easydarwin.easyrtsplive.activity;

import android.content.Intent;
import android.databinding.DataBindingUtil;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.view.View;

import org.easydarwin.easyrtsplive.R;
import org.easydarwin.easyrtsplive.TheApp;
import org.easydarwin.easyrtsplive.databinding.ActivityAboutBinding;

public class AboutActivity extends AppCompatActivity implements View.OnClickListener {
    private static final String EP_URL = "https://github.com/EasyDarwin/EasyRTSPLive";

    private ActivityAboutBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
            getWindow().setStatusBarColor(Color.TRANSPARENT);
        }

        binding = DataBindingUtil.setContentView(this, R.layout.activity_about);

        setSupportActionBar(binding.toolbar);
        binding.toolbarBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });

        // 版本信息
        binding.version.setText("EasyRTSPLive");
        binding.version.append("(");

        SpannableString ss;
        if (TheApp.activeDays >= 9999) {
            ss = new SpannableString("激活码永久有效");
            ss.setSpan(new ForegroundColorSpan(getResources().getColor(R.color.colorGREEN)),
                    0,
                    ss.length(),
                    Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        } else if (TheApp.activeDays > 0) {
            ss = new SpannableString(String.format("激活码还剩%d天可用", TheApp.activeDays));
            ss.setSpan(new ForegroundColorSpan(getResources().getColor(R.color.colorYELLOW)),
                    0,
                    ss.length(),
                    Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        } else {
            ss = new SpannableString(String.format("激活码已过期(%d)", TheApp.activeDays));
            ss.setSpan(new ForegroundColorSpan(getResources().getColor(R.color.colorRED)),
                    0,
                    ss.length(),
                    Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        }

        binding.version.append(ss);
        binding.version.append(")");

        binding.darwinTv.setText("项目地址：");
        SpannableString ss3 = new SpannableString(EP_URL);
        ss3.setSpan(new ForegroundColorSpan(getResources().getColor(R.color.colorTheme2)),
                0,
                ss3.length(),
                Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        binding.darwinTv.append(ss3);

        binding.darwinTv.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        Intent intent= new Intent();
        intent.setAction("android.intent.action.VIEW");
        Uri content_url = Uri.parse(EP_URL);

        switch (v.getId()) {
            case R.id.darwin_tv:
                content_url = Uri.parse(EP_URL);
                break;
        }

        intent.setData(content_url);
        startActivity(intent);
    }
}