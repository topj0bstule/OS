package com.example.todoapi.model;

import javax.xml.bind.annotation.adapters.XmlAdapter;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class LocalDateTimeAdapter extends XmlAdapter<String, LocalDateTime> {
    
    private static final DateTimeFormatter FORMATTER = 
        DateTimeFormatter.ISO_LOCAL_DATE_TIME;
    
    @Override
    public LocalDateTime unmarshal(String value) throws Exception {
        if (value == null || value.trim().isEmpty()) {
            return null;
        }
        return LocalDateTime.parse(value, FORMATTER);
    }
    
    @Override
    public String marshal(LocalDateTime value) throws Exception {
        if (value == null) {
            return null;
        }
        return value.format(FORMATTER);
    }
}