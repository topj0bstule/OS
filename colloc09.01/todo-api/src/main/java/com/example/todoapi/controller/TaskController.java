package com.example.todoapi.controller;

import com.example.todoapi.model.Task;
import com.example.todoapi.service.TaskService;
import com.example.todoapi.service.XmlValidationService;
import com.example.todoapi.dto.TaskRequest;
import jakarta.validation.Valid;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/tasks")
@CrossOrigin(origins = "*")
public class TaskController {
    
    @Autowired
    private TaskService taskService;
    
    @Autowired
    private XmlValidationService xmlValidationService;
    
    // GET /api/tasks - получить все задачи
    @GetMapping
    public ResponseEntity<List<Task>> getAllTasks() {
        List<Task> tasks = taskService.getAllTasks();
        return ResponseEntity.ok(tasks);
    }
    
    // GET /api/tasks/xml - получить все задачи в XML формате
    @GetMapping(value = "/xml", produces = MediaType.APPLICATION_XML_VALUE)
    public ResponseEntity<String> getAllTasksXml() {
        try {
            List<Task> tasks = taskService.getAllTasks();
            String xml = xmlValidationService.tasksToXml(tasks);
            return ResponseEntity.ok(xml);
        } catch (Exception e) {
            String errorXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                    "<error>\n" +
                    "  <message>Error: " + e.getMessage() + "</message>\n" +
                    "</error>";
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
                    .contentType(MediaType.APPLICATION_XML)
                    .body(errorXml);
        }
    }
    
    // GET /api/tasks/{id} - получить задачу по ID
    @GetMapping("/{id}")
    public ResponseEntity<?> getTaskById(@PathVariable Long id) {
        return taskService.getTaskById(id)
                .<ResponseEntity<?>>map(ResponseEntity::ok)
                .orElse(ResponseEntity.status(HttpStatus.NOT_FOUND)
                        .body(createErrorResponse("Task not found with id: " + id)));
    }
    
    // GET /api/tasks/{id}/xml - получить задачу в XML формате
    @GetMapping(value = "/{id}/xml", produces = MediaType.APPLICATION_XML_VALUE)
    public ResponseEntity<String> getTaskByIdXml(@PathVariable Long id) {
        return taskService.getTaskById(id)
                .<ResponseEntity<String>>map(task -> {
                    try {
                        String xml = xmlValidationService.taskToXml(task);
                        return ResponseEntity.ok()
                                .contentType(MediaType.APPLICATION_XML)
                                .body(xml);
                    } catch (Exception e) {
                        String errorXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                                "<error>\n" +
                                "  <message>Error: " + e.getMessage() + "</message>\n" +
                                "</error>";
                        return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
                                .contentType(MediaType.APPLICATION_XML)
                                .body(errorXml);
                    }
                })
                .orElseGet(() -> {
                    String errorXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                            "<error>\n" +
                            "  <message>Task not found with id: " + id + "</message>\n" +
                            "</error>";
                    return ResponseEntity.status(HttpStatus.NOT_FOUND)
                            .contentType(MediaType.APPLICATION_XML)
                            .body(errorXml);
                });
    }
    
    // POST /api/tasks - создать задачу
    @PostMapping
    public ResponseEntity<Task> createTask(@Valid @RequestBody TaskRequest taskRequest) {
        Task task = taskService.createTask(taskRequest);
        return ResponseEntity.status(HttpStatus.CREATED).body(task);
    }
    
    // PUT /api/tasks/{id} - полностью обновить задачу
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
    
    // PATCH /api/tasks/{id} - частично обновить задачу
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
    
    // DELETE /api/tasks/{id} - удалить задачу
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
    
    // GET /api/tasks/status/{status} - получить задачи по статусу
    @GetMapping("/status/{status}")
    public ResponseEntity<List<Task>> getTasksByStatus(@PathVariable Task.TaskStatus status) {
        List<Task> tasks = taskService.getTasksByStatus(status);
        return ResponseEntity.ok(tasks);
    }
    
    // GET /api/tasks/status/{status}/xml - получить задачи по статусу в XML
    @GetMapping(value = "/status/{status}/xml", produces = MediaType.APPLICATION_XML_VALUE)
    public ResponseEntity<String> getTasksByStatusXml(@PathVariable Task.TaskStatus status) {
        try {
            List<Task> tasks = taskService.getTasksByStatus(status);
            String xml = xmlValidationService.tasksToXml(tasks);
            return ResponseEntity.ok(xml);
        } catch (Exception e) {
            String errorXml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" +
                    "<error>\n" +
                    "  <message>Error: " + e.getMessage() + "</message>\n" +
                    "</error>";
            return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
                    .contentType(MediaType.APPLICATION_XML)
                    .body(errorXml);
        }
    }
    
    // GET /api/tasks/validate - ручная валидация всех задач
    @GetMapping("/validate")
    public ResponseEntity<?> validateAllTasks() {
        try {
            List<Task> tasks = taskService.getAllTasks();
            
            // Пробуем сгенерировать XML
            String xml = xmlValidationService.tasksToXml(tasks);
            
            // Проверяем валидность
            boolean isValid = xmlValidationService.validateXml(xml);
            
            Map<String, Object> response = new HashMap<>();
            response.put("success", true);
            response.put("valid", isValid);
            response.put("message", isValid ? 
                "All " + tasks.size() + " tasks passed XML validation" : 
                "Warning: XML may not be schema-valid");
            response.put("taskCount", tasks.size());
            response.put("timestamp", System.currentTimeMillis());
            
            return ResponseEntity.ok(response);
        } catch (Exception e) {
            return ResponseEntity.status(HttpStatus.BAD_REQUEST)
                    .body(createErrorResponse("XML generation failed: " + e.getMessage()));
        }
    }
    
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