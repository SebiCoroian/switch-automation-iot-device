package com.sebastiancoroian.graduationproject.models;

import com.google.gson.annotations.SerializedName;

public class Device {
    @SerializedName(value = "id")
    public String deviceID;
    @SerializedName(value = "name")
    public String name;
    @SerializedName(value = "state")
    public String state;
}

