package com.sebastiancoroian.graduationproject.services;

import com.sebastiancoroian.graduationproject.models.Device;

import java.util.List;

import retrofit2.Call;
import retrofit2.http.GET;
import retrofit2.http.Path;

public interface DeviceService {
    @GET("push/{device_name}")
    Call<Void> push(@Path("device_name") String deviceName);
    @GET("pull/{device_name}")
    Call<Void> pull(@Path("device_name") String deviceName);
    @GET("devices")
    Call<List<Device>> getDevices();
}

