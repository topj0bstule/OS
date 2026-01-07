package com.example.todoapi.model;

import jakarta.persistence.*;
import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Size;
import java.time.LocalDateTime;

@Entity
@Table(name = "tasks")
public class Task {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @NotBlank(message = "Title is mandatory")
    @Size(min = 3, max = 100, message = "Title must be between 3 and 100 characters")
    @Column(nullable = false)
    private String title;
    
    @Size(max = 500, message = "Description cannot exceed 500 characters")
    private String description;
    
    @NotNull(message = "Status is mandatory")
    @Enumerated(EnumType.STRING)
    private TaskStatus status;
    
    @Column(name = "created_at", updatable = false)
    private LocalDateTime createdAt;
    
    @Column(name = "updated_at")
    private LocalDateTime updatedAt;
    
    // Enum для статусов
    public enum TaskStatus {
        TODO, IN_PROGRESS, DONE
    }
    
    // Конструкторы
    public Task() {
        this.createdAt = LocalDateTime.now();
        this.updatedAt = LocalDateTime.now();
    }
    
    public Task(String title, String description, TaskStatus status) {
        this.title = title;
        this.description = description;
        this.status = status;
        this.createdAt = LocalDateTime.now();
        this.updatedAt = LocalDateTime.now();
    }
    
    // Геттеры и сеттеры
    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }
    
    public String getTitle() { return title; }
    public void setTitle(String title) { 
        this.title = title; 
        this.updatedAt = LocalDateTime.now();
    }
    
    public String getDescription() { return description; }
    public void setDescription(String description) { 
        this.description = description; 
        this.updatedAt = LocalDateTime.now();
    }
    
    public TaskStatus getStatus() { return status; }
    public void setStatus(TaskStatus status) { 
        this.status = status; 
        this.updatedAt = LocalDateTime.now();
    }
    
    public LocalDateTime getCreatedAt() { return createdAt; }
    public void setCreatedAt(LocalDateTime createdAt) { this.createdAt = createdAt; }
    
    public LocalDateTime getUpdatedAt() { return updatedAt; }
    public void setUpdatedAt(LocalDateTime updatedAt) { this.updatedAt = updatedAt; }
    
    // PrePersist и PreUpdate
    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
        updatedAt = LocalDateTime.now();
    }
    
    @PreUpdate
    protected void onUpdate() {
        updatedAt = LocalDateTime.now();
    }
}