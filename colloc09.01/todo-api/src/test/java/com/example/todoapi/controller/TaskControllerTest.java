package com.example.todoapi.controller;

import com.example.todoapi.model.Task;
import com.example.todoapi.service.XmlValidationService;
import com.example.todoapi.service.TaskService;
import com.example.todoapi.dto.TaskRequest;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.web.servlet.WebMvcTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.http.MediaType;
import org.springframework.test.web.servlet.MockMvc;

import java.util.Arrays;
import java.util.Optional;

import static org.hamcrest.Matchers.*;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.*;
import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.*;
import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;

@WebMvcTest(TaskController.class)
public class TaskControllerTest {
    
    @Autowired
    private MockMvc mockMvc;
    
    @MockBean
    private TaskService taskService;
    
    @MockBean
    private XmlValidationService xmlValidationService;
    
    @Autowired
    private ObjectMapper objectMapper;
    
    @Test
    void getAllTasks_ShouldReturnTasks() throws Exception {
        // Arrange
        Task task1 = new Task("Task 1", "Description 1", Task.TaskStatus.TODO);
        task1.setId(1L);
        Task task2 = new Task("Task 2", "Description 2", Task.TaskStatus.IN_PROGRESS);
        task2.setId(2L);
        
        when(taskService.getAllTasks()).thenReturn(Arrays.asList(task1, task2));
        
        // Act & Assert
        mockMvc.perform(get("/api/tasks"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$", hasSize(2)))
                .andExpect(jsonPath("$[0].title", is("Task 1")))
                .andExpect(jsonPath("$[0].status", is("TODO")))
                .andExpect(jsonPath("$[1].title", is("Task 2")))
                .andExpect(jsonPath("$[1].status", is("IN_PROGRESS")));
        
        verify(taskService, times(1)).getAllTasks();
    }
    
    @Test
    void getTaskById_ExistingId_ShouldReturnTask() throws Exception {
        // Arrange
        Task task = new Task("Test Task", "Test Description", Task.TaskStatus.DONE);
        task.setId(1L);
        
        when(taskService.getTaskById(1L)).thenReturn(Optional.of(task));
        
        // Act & Assert
        mockMvc.perform(get("/api/tasks/1"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.id", is(1)))
                .andExpect(jsonPath("$.title", is("Test Task")))
                .andExpect(jsonPath("$.status", is("DONE")));
        
        verify(taskService, times(1)).getTaskById(1L);
    }
    
    @Test
    void getTaskById_NonExistingId_ShouldReturn404() throws Exception {
        // Arrange
        when(taskService.getTaskById(999L)).thenReturn(Optional.empty());
        
        // Act & Assert
        mockMvc.perform(get("/api/tasks/999"))
                .andExpect(status().isNotFound())
                .andExpect(jsonPath("$.success", is(false)))
                .andExpect(jsonPath("$.message", containsString("not found")));
        
        verify(taskService, times(1)).getTaskById(999L);
    }
    
    @Test
    void createTask_ValidRequest_ShouldReturnCreatedTask() throws Exception {
        // Arrange
        TaskRequest request = new TaskRequest("New Task", "New Description", Task.TaskStatus.TODO);
        Task createdTask = new Task("New Task", "New Description", Task.TaskStatus.TODO);
        createdTask.setId(1L);
        
        when(taskService.createTask(any(TaskRequest.class))).thenReturn(createdTask);
        
        // Act & Assert
        mockMvc.perform(post("/api/tasks")
                .contentType(MediaType.APPLICATION_JSON)
                .content(objectMapper.writeValueAsString(request)))
                .andExpect(status().isCreated())
                .andExpect(jsonPath("$.id", is(1)))
                .andExpect(jsonPath("$.title", is("New Task")))
                .andExpect(jsonPath("$.status", is("TODO")));
        
        verify(taskService, times(1)).createTask(any(TaskRequest.class));
    }
    
    @Test
    void deleteTask_ExistingId_ShouldReturnSuccess() throws Exception {
        // Arrange - ничего не нужно, метод void
        
        // Act & Assert
        mockMvc.perform(delete("/api/tasks/1"))
                .andExpect(status().isOk())
                .andExpect(jsonPath("$.success", is(true)))
                .andExpect(jsonPath("$.message", containsString("deleted")));
        
        verify(taskService, times(1)).deleteTask(1L);
    }
    
    @Test
    void deleteTask_NonExistingId_ShouldReturn404() throws Exception {
        // Arrange
        doThrow(new RuntimeException("Task not found with id: 999"))
            .when(taskService).deleteTask(999L);
        
        // Act & Assert
        mockMvc.perform(delete("/api/tasks/999"))
                .andExpect(status().isNotFound())
                .andExpect(jsonPath("$.success", is(false)))
                .andExpect(jsonPath("$.message", containsString("not found")));
        
        verify(taskService, times(1)).deleteTask(999L);
    }
}