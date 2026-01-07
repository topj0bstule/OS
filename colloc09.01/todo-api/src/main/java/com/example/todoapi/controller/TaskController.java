package com.example.todoapi.controller;

import com.example.todoapi.model.Task;
import com.example.todoapi.service.TaskService;
import com.example.todoapi.dto.TaskRequest;
import jakarta.validation.Valid;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

@RestController
@RequestMapping("/tasks")
@CrossOrigin(origins = "*")
public class TaskController {
    
    @Autowired
    private TaskService taskService;
    
    // GET /tasks - получить все задачи
    @GetMapping
    public ResponseEntity<List<Task>> getAllTasks() {
        List<Task> tasks = taskService.getAllTasks();
        return ResponseEntity.ok(tasks);
    }
    
    // GET /tasks/{id} - получить задачу по ID (ИСПРАВЛЕНО)
    @GetMapping("/{id}")
    public ResponseEntity<?> getTaskById(@PathVariable Long id) {
        return taskService.getTaskById(id)
                .<ResponseEntity<?>>map(ResponseEntity::ok)
                .orElse(ResponseEntity.status(HttpStatus.NOT_FOUND)
                        .body(createErrorResponse("Task not found with id: " + id)));
    }
    
    // POST /tasks - создать задачу
    @PostMapping
    public ResponseEntity<Task> createTask(@Valid @RequestBody TaskRequest taskRequest) {
        Task task = taskService.createTask(taskRequest);
        return ResponseEntity.status(HttpStatus.CREATED).body(task);
    }
    
    // PUT /tasks/{id} - полностью обновить задачу
    @PutMapping("/{id}")
    public ResponseEntity<?> updateTask(@PathVariable Long id, 
                                         @Valid @RequestBody TaskRequest taskRequest) {
        try {
            Task updatedTask = taskService.updateTask(id, taskRequest);
            return ResponseEntity.ok(updatedTask);
        } catch (RuntimeException e) {
            return ResponseEntity.status(HttpStatus.NOT_FOUND)
                    .body(createErrorResponse(e.getMessage()));
        }
    }
    
    // PATCH /tasks/{id} - частично обновить задачу
    @PatchMapping("/{id}")
    public ResponseEntity<?> updateTaskPartial(@PathVariable Long id,
                                                @RequestBody TaskRequest taskRequest) {
        try {
            Task updatedTask = taskService.updateTaskPartial(id, taskRequest);
            return ResponseEntity.ok(updatedTask);
        } catch (RuntimeException e) {
            return ResponseEntity.status(HttpStatus.NOT_FOUND)
                    .body(createErrorResponse(e.getMessage()));
        }
    }
    
    // DELETE /tasks/{id} - удалить задачу
    @DeleteMapping("/{id}")
    public ResponseEntity<?> deleteTask(@PathVariable Long id) {
        try {
            taskService.deleteTask(id);
            return ResponseEntity.ok(createSuccessResponse("Task deleted successfully"));
        } catch (RuntimeException e) {
            return ResponseEntity.status(HttpStatus.NOT_FOUND)
                    .body(createErrorResponse(e.getMessage()));
        }
    }
    
    // GET /tasks/status/{status} - получить задачи по статусу
    @GetMapping("/status/{status}")
    public ResponseEntity<List<Task>> getTasksByStatus(@PathVariable Task.TaskStatus status) {
        List<Task> tasks = taskService.getTasksByStatus(status);
        return ResponseEntity.ok(tasks);
    }
    
    // Вспомогательные методы для ответов
    private Map<String, Object> createErrorResponse(String message) {
        Map<String, Object> response = new HashMap<>();
        response.put("success", false);
        response.put("message", message);
        response.put("timestamp", System.currentTimeMillis());
        return response;
    }
    
    private Map<String, Object> createSuccessResponse(String message) {
        Map<String, Object> response = new HashMap<>();
        response.put("success", true);
        response.put("message", message);
        response.put("timestamp", System.currentTimeMillis());
        return response;
    }
}