package com.sebastiancoroian.graduationproject.adapters;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;

import com.sebastiancoroian.graduationproject.R;
import com.sebastiancoroian.graduationproject.models.Device;
import com.sebastiancoroian.graduationproject.services.DeviceService;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class DeviceListAdapter extends BaseAdapter {

    private static final String TAG = "DeviceListAdapter";

    private final List<Device> items;
    private final DeviceService deviceService;

    public DeviceListAdapter(DeviceService deviceService) {
        this.items = new ArrayList<>();
        this.deviceService = deviceService;
        reloadItems();
    }

    public void reloadItems() {
        this.deviceService.getDevices().enqueue(new Callback<List<Device>>() {
            @Override
            public void onResponse(Call<List<Device>> call, Response<List<Device>> response) {
                items.clear();
                if (response.body() == null) {
                    Log.e(TAG, "received empty device list body");
                    return;
                }
                items.addAll(response.body());
                notifyDataSetChanged();
            }
            @Override
            public void onFailure(Call<List<Device>> call, Throwable t) {
                Log.e(TAG, "onFailure: failed to get device list", t);
                items.clear();
                notifyDataSetChanged();
            }
        });
    }

    @Override
    public int getCount() {
        return items.size();
    }

    @Override
    public Object getItem(int i) {
        return items.get(i);
    }

    @Override
    public long getItemId(int i) {
        return getItem(i).hashCode();
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        if (view == null) {
            LayoutInflater inflater = LayoutInflater.from(viewGroup.getContext());
            view = inflater.inflate(R.layout.device_list_item, viewGroup, false);
        }
        final Device device = items.get(i);
        final TextView deviceNameTextView = (TextView) view.findViewById(R.id.device_name_text_view);
        if (device.name.isEmpty()) {
            deviceNameTextView.setText(device.deviceID);
        } else {
            deviceNameTextView.setText(device.name);
        }
        final TextView deviceStatusTextView = (TextView) view.findViewById(R.id.device_status_text_view);
        deviceStatusTextView.setText(device.state);
        final Button devicePushButton = (Button) view.findViewById(R.id.device_push_button);
        devicePushButton.setOnClickListener(devicePushButtonView -> {
            Call<Void> call = deviceService.push(device.deviceID);
            call.enqueue(new Callback<Void>() {
                @Override
                public void onResponse(Call<Void> call, Response<Void> response) {
                    reloadItems();
                }
                @Override
                public void onFailure(Call<Void> call, Throwable t) {
                    Log.e(TAG, "onFailure: device push failed", t);
                    t.printStackTrace();
                    reloadItems();
                }
            });
        });

        final Button devicePullButton = (Button) view.findViewById(R.id.device_pull_button);
        devicePullButton.setOnClickListener(devicePullButtonView -> {
            Call<Void> call = deviceService.pull(device.deviceID);
            call.enqueue(new Callback<Void>() {
                @Override
                public void onResponse(Call<Void> call, Response<Void> response) {
                    reloadItems();
                }
                @Override
                public void onFailure(Call<Void> call, Throwable t) {
                    Log.e(TAG, "onFailure: device pull failed", t);
                    t.printStackTrace();
                    reloadItems();
                }
            });
        });

        return view;
    }
}
