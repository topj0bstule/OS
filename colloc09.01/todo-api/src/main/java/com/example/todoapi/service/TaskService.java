package com.example.todoapi.service;

import com.example.todoapi.model.Task;
import com.example.todoapi.repository.TaskRepository;
import com.example.todoapi.dto.TaskRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import java.util.List;
import java.util.Optional;

@Service
@Transactional
public class TaskService {
    
    @Autowired
    private TaskRepository taskRepository;
    
    // Получить все задачи
    public List<Task> getAllTasks() {
        return taskRepository.findAll();
    }
    
    // Получить задачу по ID
    public Optional<Task> getTaskById(Long id) {
        return taskRepository.findById(id);
    }
    
    // Создать задачу
    public Task createTask(TaskRequest taskRequest) {
        Task task = new Task();
        task.setTitle(taskRequest.getTitle());
        task.setDescription(taskRequest.getDescription());
        task.setStatus(taskRequest.getStatus());
        
        return taskRepository.save(task);
    }
    
    // Обновить задачу
    public Task updateTask(Long id, TaskRequest taskRequest) {
        Task task = taskRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("Task not found with id: " + id));
        
        task.setTitle(taskRequest.getTitle());
        task.setDescription(taskRequest.getDescription());
        task.setStatus(taskRequest.getStatus());
        
        return taskRepository.save(task);
    }
    
    // Частичное обновление (PATCH)
    public Task updateTaskPartial(Long id, TaskRequest taskRequest) {
        Task task = taskRepository.findById(id)
            .orElseThrow(() -> new RuntimeException("Task not found with id: " + id));
        
        if (taskRequest.getTitle() != null) {
            task.setTitle(taskRequest.getTitle());
        }
        if (taskRequest.getDescription() != null) {
            task.setDescription(taskRequest.getDescription());
        }
        if (taskRequest.getStatus() != null) {
            task.setStatus(taskRequest.getStatus());
        }
        
        return taskRepository.save(task);
    }
    
    // Удалить задачу
    public void deleteTask(Long id) {
        if (!taskRepository.existsById(id)) {
            throw new RuntimeException("Task not found with id: " + id);
        }
        taskRepository.deleteById(id);
    }
    
    // Получить задачи по статусу
    public List<Task> getTasksByStatus(Task.TaskStatus status) {
        return taskRepository.findByStatus(status);
    }
}