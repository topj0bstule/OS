package com.example.todoapi.dto;

import com.example.todoapi.model.Task;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Size;

public class TaskRequest {
    
    @NotBlank(message = "Title is mandatory")
    @Size(min = 3, max = 100, message = "Title must be between 3 and 100 characters")
    private String title;
    
    @Size(max = 500, message = "Description cannot exceed 500 characters")
    private String description;
    
    @NotNull(message = "Status is mandatory")
    private Task.TaskStatus status;
    
    // Конструкторы
    public TaskRequest() {}
    
    public TaskRequest(String title, String description, Task.TaskStatus status) {
        this.title = title;
        this.description = description;
        this.status = status;
    }
    
    // Геттеры и сеттеры
    public String getTitle() { return title; }
    public void setTitle(String title) { this.title = title; }
    
    public String getDescription() { return description; }
    public void setDescription(String description) { this.description = description; }
    
    public Task.TaskStatus getStatus() { return status; }
    public void setStatus(Task.TaskStatus status) { this.status = status; }
}